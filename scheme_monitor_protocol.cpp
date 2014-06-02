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

#include "scheme_monitor_protocol.h"

#include <functional>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "errors.h"
#include "scheme_common.h"
#include "string_encoder.h"

namespace asio = boost::asio;

namespace decof
{

scheme_monitor_protocol::scheme_monitor_protocol(object_dictionary& object_dictionary, const tcp::endpoint &endpoint) :
    client_proxy(object_dictionary),
    acceptor_(object_dictionary.get_io_service(), endpoint),
    socket_(object_dictionary.get_io_service())
{}

void scheme_monitor_protocol::preload()
{
    if (socket_.is_open())
        socket_.close();

    // Make 1st asynchronous call
    acceptor_.async_accept(
        socket_,
        std::bind(&scheme_monitor_protocol::accept_handler, this, std::placeholders::_1)
    );
}

void scheme_monitor_protocol::accept_handler(boost::system::error_code error)
{
    if (!error) {
        // Prepare for data reception
        read_next();
    } else
        throw std::runtime_error(error.message());
}

void scheme_monitor_protocol::read_next()
{
    // Connection accepted, start async reads
    asio::async_read_until(
        socket_,
        inbuf_,
        '\n',
        std::bind(&scheme_monitor_protocol::read_handler, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void scheme_monitor_protocol::read_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        // First copy bytes_transferred bytes from the streambuf to a
        // std::string. This is one of the uglyest things in boost::asio and only
        // properly documented in stackoverflow:
        // http://stackoverflow.com/questions/877652/copy-a-streambufs-contents-to-a-string.
        boost::asio::streambuf::const_buffers_type buffer = inbuf_.data();
        std::string str(boost::asio::buffers_begin(buffer), boost::asio::buffers_begin(buffer) + bytes_transferred);
        inbuf_.consume(bytes_transferred);

        // Trim (whitespace as in std::is_space() and parantheses) and tokenize the request string
        boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v()"));
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, str, boost::algorithm::is_space(), boost::algorithm::token_compress_on);

        try {
            if (tokens.size() <= 1)
                throw parse_error();

            // Lower-case first substring
            std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::tolower);

            // Remove optional "'" from parameter name
            if (tokens[1][0] == '\'')
                tokens[1].erase(0, 1);

            if (tokens[0] == "subscribe" || tokens[0] == "add") {
                //observe(tokens[1], boost::bind(&scheme_monitor_protocol::notify, this, _1, _2));
                observe(tokens[1], std::bind(&scheme_monitor_protocol::notify, this, std::placeholders::_1, std::placeholders::_2));
            } else if (tokens[0] == "unsubscribe" || tokens[0] == "remove") {
                unobserve(tokens[1]);
            } else
                throw unknown_operation_error();
        } catch (runtime_error& ex) {
            std::stringstream ss;
            ss << "ERROR " << ex.code() << ": " << ex.what() << std::endl;
            write_next(ss.str());
        }

        read_next();
    } else if (error.value() == asio::error::eof) {
        // Connection was closed by peer
        preload();
    } else
        throw std::runtime_error(error.message());
}

void scheme_monitor_protocol::write_next(std::string str)
{
    std::ostream os(&outbuf_);
    os << str;

    asio::async_write(
        socket_,
        outbuf_,
        std::bind(&scheme_monitor_protocol::write_handler, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void scheme_monitor_protocol::write_handler(const boost::system::error_code &error, std::size_t)
{
    if (error.value() == asio::error::eof) {
        // Connection was closed by peer
        preload();
    } else if (error)
        throw std::runtime_error(error.message());
}

void scheme_monitor_protocol::notify(const std::string &uri, const boost::any &any_value) noexcept
{
    // Get current time in textual representation
    const size_t max_length = 25;
    char time_str[max_length];
    std::time_t now = std::time(nullptr);
    std::strftime(time_str, sizeof(time_str), "%FT%TZ", std::localtime(&now));

    write_next(std::string("(") + time_str + " '" + uri + " " + string_encoder::encode(any_value) + ")\n");
}

} // namespace decof
