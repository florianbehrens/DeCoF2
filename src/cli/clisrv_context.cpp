/*
 * Copyright (c) 2014 Florian Behrens
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <decof/cli/clisrv_context.h>

#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/any.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include <decof/exceptions.h>
#include <decof/object.h>
#include <decof/object_dictionary.h>

#include "parser.h"
#include "encoder.h"
#include "visitor.h"

namespace
{

const std::string prompt("> ");

} // anonymous namespace

namespace decof
{

namespace cli
{

clisrv_context::clisrv_context(boost::asio::ip::tcp::socket&& socket, object_dictionary& od, userlevel_t userlevel) :
    client_context(od, userlevel),
    socket_(std::move(socket))
{}

std::string clisrv_context::connection_type() const
{
    return std::string("tcp");
}

std::string clisrv_context::remote_endpoint() const
{
    return boost::lexical_cast<std::string>(socket_.remote_endpoint());
}

void clisrv_context::preload()
{
    std::ostream out(&outbuf_);
    out << "DeCoF command line\n" << prompt;

    auto self(std::dynamic_pointer_cast<clisrv_context>(shared_from_this()));
    boost::asio::async_write(socket_, outbuf_,
                             std::bind(&clisrv_context::write_handler, self,
                                       std::placeholders::_1, std::placeholders::_2));
}

void clisrv_context::write_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        auto self(std::dynamic_pointer_cast<clisrv_context>(shared_from_this()));
        boost::asio::async_read_until(socket_, inbuf_, '\n',
                                      std::bind(&clisrv_context::read_handler, self,
                                                std::placeholders::_1, std::placeholders::_2));
    } else
        disconnect();
}

void clisrv_context::read_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        // Copy received data to std::string
        boost::asio::streambuf::const_buffers_type bufs = inbuf_.data();
        std::string str(
            boost::asio::buffers_begin(bufs),
            boost::asio::buffers_begin(bufs) + bytes_transferred);
        inbuf_.consume(bytes_transferred);

        // Trim (whitespace as in std::is_space() and parantheses) and tokenize the request string
        boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v()"));

        // Read and lower-case operation and uri
        std::string op, uri;
        std::stringstream ss_in(str);
        ss_in >> op >> uri;
        std::transform(op.begin(), op.end(), op.begin(), ::tolower);
        std::transform(uri.begin(), uri.end(), uri.begin(), ::tolower);

        // Remove optional "'" from parameter name
        if (!uri.empty() && uri[0] == '\'')
            uri.erase(0, 1);

        // Parse optional value string using flexc++/bisonc++ parser
        boost::any any_value;
        if (ss_in.peek() != std::stringstream::traits_type::eof()) {
            parser parser(ss_in);
            parser.parse();
            any_value = parser.result();
        }

        std::ostream out(&outbuf_);

        try {
            if ((op == "get" || op == "param-ref") && !uri.empty() && any_value.empty()) {
                boost::any any_value = get_parameter(uri);
                encoder().encode_any(out, any_value);
                out << std::endl;
            } else if ((op == "set" || op == "param-set!") && !uri.empty() && !any_value.empty()) {
                set_parameter(uri, any_value);
                out << "0\n";
            } else if ((op == "signal" || op == "exec") && !uri.empty() && any_value.empty()) {
                signal_event(uri);
                out << "()\n";
            } else if ((op == "browse" || op == "param-disp") && any_value.empty()) {
                // This command is for compatibility reasons with legacy DeCoF
                std::string root_uri(object_dictionary_.name());
                if (!uri.empty())
                    root_uri = uri;
                std::stringstream temp_ss;
                visitor visitor(temp_ss);
                browse(&visitor, root_uri);
                out << temp_ss.str();
            } else
                throw parse_error();
        } catch (runtime_error& ex) {
            out << "ERROR " << ex.code() << ": " << ex.what() << std::endl;
        }

        out << prompt;

        auto self(std::dynamic_pointer_cast<clisrv_context>(shared_from_this()));
        boost::asio::async_write(socket_, outbuf_,
                                 std::bind(&clisrv_context::write_handler, self,
                                           std::placeholders::_1, std::placeholders::_2));
    }
    else
        disconnect();
}

void clisrv_context::disconnect()
{
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);

    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto sptr = shared_from_this();
    object_dictionary_.remove_context(sptr);
}

} // namespace cli

} // namespace decof
