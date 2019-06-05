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

#include "browse_visitor.h"
#include "encoder.h"
#include "parser.h"
#include "tree_visitor.h"
#include <decof/cli/clisrv_context.h>
#include <decof/exceptions.h>
#include <decof/object.h>
#include <decof/object_dictionary.h>
#include <decof/types.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/any.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/lexical_cast.hpp>
#include <limits>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

using boost::system::error_code;

namespace {

const std::string prompt("> ");

} // anonymous namespace

namespace decof {

namespace cli {

clisrv_context::clisrv_context(strand_t& strand, socket_t&& socket, object_dictionary& od, userlevel_t userlevel)
  : cli_context_base(od, userlevel), strand_(strand), socket_(std::move(socket))
{
    if (connect_event_cb_)
        connect_event_cb_(false, true, remote_endpoint());
}

std::string clisrv_context::connection_type() const
{
    return std::string("tcp");
}

std::string clisrv_context::remote_endpoint() const
{
    error_code ec;
    return boost::lexical_cast<std::string>(socket_.remote_endpoint(ec));
}

void clisrv_context::preload()
{
    std::ostream out(&outbuf_);
    out << "DeCoF command line\n" << prompt;

    auto self = shared_from_this();
    boost::asio::async_write(socket_, outbuf_, strand_.wrap([self](const error_code& err, std::size_t bytes) {
        self->write_handler(err, bytes);
    }));
}

void clisrv_context::write_handler(const error_code& error, std::size_t)
{
    if (!error) {
        auto self = shared_from_this();
        boost::asio::async_read_until(
            socket_, inbuf_, '\n', strand_.wrap([self](const error_code& err, std::size_t bytes) {
                self->read_handler(err, bytes);
            }));
    } else
        disconnect();
}

void clisrv_context::read_handler(const error_code& error, std::size_t bytes_transferred)
{
    if (!error) {
        auto bufs = inbuf_.data();
        process_request(
            std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + bytes_transferred));
        inbuf_.consume(bytes_transferred);

        auto self = shared_from_this();
        boost::asio::async_write(socket_, outbuf_, strand_.wrap([self](const error_code& err, std::size_t bytes) {
            self->write_handler(err, bytes);
        }));
    } else
        disconnect();
}

void clisrv_context::disconnect()
{
    if (connect_event_cb_)
        connect_event_cb_(false, false, remote_endpoint());

    error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);
}

void clisrv_context::process_request(std::string request)
{
    // Trim whitespace and parantheses
    std::string trimmed_request = boost::algorithm::trim_copy_if(request, boost::is_any_of(" \f\n\r\t\v()"));

    // Ignore empty request
    if (trimmed_request.empty())
        return;

    // Read operation and uri
    std::string       op, uri;
    std::stringstream ss_in(trimmed_request);
    ss_in >> op >> uri;
    std::transform(op.begin(), op.end(), op.begin(), ::tolower);

    // Remove optional "'" from parameter name
    if (!uri.empty() && uri[0] == '\'')
        uri.erase(0, 1);

    std::ostream out(&outbuf_);
    encoder      encoder(out);

    try {
        // Apply special handling for the 'change-ul' command
        // (exec 'change-ul <userlevel> "<passwd>")
        if (op == "exec" && uri == "change-ul") {
            int         ul = std::numeric_limits<int>::max();
            std::string password;

            ss_in >> ul >> std::ws;
            std::getline(ss_in, password);
            boost::algorithm::trim_if(password, boost::is_any_of("\""));

            if (!userlevel_cb_(*this, static_cast<userlevel_t>(ul), password))
                throw access_denied_error();

            client_context::userlevel(static_cast<userlevel_t>(ul));

            encoder(static_cast<integer_t>(userlevel()));
            out << "\n";
        } else {
            // Parse optional value string using flexc++/bisonc++ parser
            bool    value_available = false;
            value_t value;

            if (ss_in.peek() != std::stringstream::traits_type::eof()) {
                parser parser(ss_in);
                parser.parse();
                value           = parser.result();
                value_available = true;
            }

            if ((op == "get" || op == "param-ref") && !uri.empty() && !value_available) {
                if (request_cb_)
                    request_cb_(request_t::get, request, remote_endpoint());

                // Apply special handling for 'ul' parameter
                if (uri == "ul") {
                    encoder(static_cast<integer_t>(userlevel()));
                } else {
                    const auto value = get_parameter(object_dictionary_.find_descendant_object(uri));
                    std::visit(encoder, static_cast<const value_t>(value));
                }

                out << "\n";
            } else if ((op == "set" || op == "param-set!") && !uri.empty() && value_available) {
                if (request_cb_)
                    request_cb_(request_t::set, request, remote_endpoint());

                const auto obj = object_dictionary_.find_descendant_object(uri);
                set_parameter(obj, value);

                out << "0\n";
            } else if ((op == "signal" || op == "exec") && !uri.empty() && !value_available) {
                if (request_cb_)
                    request_cb_(request_t::signal, request, remote_endpoint());

                const auto obj = object_dictionary_.find_descendant_object(uri);
                signal_event(obj);

                out << "()\n";
            } else if ((op == "browse" || op == "param-disp") && !value_available) {
                if (request_cb_)
                    request_cb_(request_t::browse, request, remote_endpoint());

                object* obj = &object_dictionary_;
                if (!uri.empty()) {
                    obj = object_dictionary_.find_descendant_object(uri);
                }

                std::ostringstream temp_ss;
                browse_visitor     visitor(temp_ss);

                browse_object(obj, &visitor);

                out << temp_ss.str();
            } else if (op == "tree" && !value_available) {
                if (request_cb_)
                    request_cb_(request_t::tree, request, remote_endpoint());

                object* obj = &object_dictionary_;
                if (!uri.empty()) {
                    obj = object_dictionary_.find_descendant_object(uri);
                }

                std::ostringstream temp_ss;
                tree_visitor       visitor(temp_ss);

                browse_object(obj, &visitor);

                out << temp_ss.str();
            } else
                throw parse_error();
        }
    } catch (runtime_error& ex) {
        out << "ERROR " << ex.code() << ": " << ex.what() << "\n";
    } catch (...) {
        out << "ERROR " << UNKNOWN_ERROR << ": "
            << "Unknown error\n";
    }

    out << prompt;
}

} // namespace cli

} // namespace decof
