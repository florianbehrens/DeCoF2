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

#include "encoder.h"
#include <decof/cli/pubsub_context.h>
#include <decof/exceptions.h>
#include <decof/object_dictionary.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string>
#include <variant>

using boost::system::error_code;

namespace {

struct iso8601_time
{
    std::chrono::time_point<std::chrono::system_clock> time_point;
};

std::ostream& operator<<(std::ostream& out, const iso8601_time& arg)
{
    std::time_t t = std::chrono::system_clock::to_time_t(arg.time_point);
    out << std::put_time(std::localtime(&t), "%FT%T.000Z");
    return out;
}

} // Anonymous namespace

namespace decof {

namespace cli {

pubsub_context::pubsub_context(strand_t& strand, socket_t&& socket, object_dictionary& od, userlevel_t userlevel)
  : cli_context_base(od, userlevel), strand_(strand), socket_(std::move(socket))
{
    if (connect_event_cb_)
        connect_event_cb_(true, true, remote_endpoint());

    boost::asio::socket_base::send_buffer_size option;
    socket_.get_option(option);
    socket_send_buf_size_ = option.value();
}

std::string pubsub_context::connection_type() const
{
    return std::string("tcp");
}

std::string pubsub_context::remote_endpoint() const
{
    error_code ec;
    return boost::lexical_cast<std::string>(socket_.remote_endpoint(ec));
}

void pubsub_context::preload()
{
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, inbuf_, '\n', strand_.wrap([self](const error_code& err, std::size_t bytes) {
        self->read_handler(err, bytes);
    }));
}

void pubsub_context::read_handler(const error_code& error, std::size_t bytes_transferred)
{
    if (!error) {
        boost::asio::streambuf::const_buffers_type bufs = inbuf_.data();

        process_request(
            std::string(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + bytes_transferred));
        inbuf_.consume(bytes_transferred);

        preload();
    } else
        close();
}

void pubsub_context::write_handler(const error_code& error, std::size_t bytes_transferred)
{
    if (!error) {
        writing_active_ = false;
        preload_writing();
    } else
        close();
}

void pubsub_context::notify(std::string uri, const value_t& value)
{
    // Cut root node name (for compatibility reasons to 'classic' DeCoF)
    if (uri != object_dictionary_.name())
        uri.erase(0, object_dictionary_.name().size() + 1);

    pending_updates_.push(uri, value);
    preload_writing();
}

void pubsub_context::preload_writing()
{
    if (writing_active_)
        return;

    std::ostream out(&outbuf_);

    while (!pending_updates_.empty() && outbuf_.size() < socket_send_buf_size_) {
        update_container::key_type   uri;
        update_container::time_point time;
        value_t                      value;

        std::tie(uri, value, time) = pending_updates_.pop_front();

        out << "(" << iso8601_time{time} << " '" << uri << " ";
        std::visit(encoder(out), value);
        out << ")\n";
    }

    if (outbuf_.size() == 0)
        return;

    auto self = shared_from_this();
    boost::asio::async_write(socket_, outbuf_, strand_.wrap([self](const error_code& err, std::size_t bytes) {
        self->write_handler(err, bytes);
    }));

    writing_active_ = true;
}

void pubsub_context::close()
{
    if (connect_event_cb_)
        connect_event_cb_(true, false, remote_endpoint());

    if (!socket_.is_open())
        return;

    socket_.close();
}

void pubsub_context::process_request(std::string request)
{
    std::string uri;

    try {
        // Trim whitespace and parantheses
        std::string trimmed_request = boost::algorithm::trim_copy_if(request, boost::is_any_of(" \f\n\r\t\v()"));

        std::stringstream in(trimmed_request);
        std::string       command;

        in >> command;

        if (command == "change-ul") {
            int         ul = std::numeric_limits<int>::max();
            std::string password;

            in >> ul >> std::ws;
            std::getline(in, password);
            boost::algorithm::trim_if(password, boost::is_any_of("\""));

            if (!userlevel_cb_(*this, static_cast<userlevel_t>(ul), password))
                throw access_denied_error();

            client_context::userlevel(static_cast<userlevel_t>(ul));
            notify(object_dictionary_.name() + ":ul", scalar_t(static_cast<decof::integer_t>(userlevel())));
        } else {
            in >> uri;

            // Lower-case first substring
            std::transform(command.begin(), command.end(), command.begin(), ::tolower);

            // Remove optional "'" from parameter name
            if (uri[0] == '\'')
                uri.erase(0, 1);

            if (command == "subscribe" || command == "add") {
                if (request_cb_)
                    request_cb_(request_t::subscribe, request, remote_endpoint());

                // Apply special handling for 'ul' parameter
                if (uri == "ul") {
                    notify(object_dictionary_.name() + ":ul", static_cast<decof::integer_t>(userlevel()));
                } else {
                    auto obj = object_dictionary_.find_descendant_object(uri);
                    observe(
                        obj, std::bind(&pubsub_context::notify, this, std::placeholders::_1, std::placeholders::_2));
                }
            } else if (command == "unsubscribe" || command == "remove") {
                if (request_cb_)
                    request_cb_(request_t::unsubscribe, request, remote_endpoint());

                auto obj = object_dictionary_.find_descendant_object(uri);
                unobserve(obj);
            } else
                throw unknown_operation_error();
        }
    } catch (invalid_parameter_error& ex) {
        std::ostream out(&outbuf_);
        out << "(Error: " << ex.code() << " (" << iso8601_time{std::chrono::system_clock::now()}
            << " 'COMMAND_ERROR) Parameter '" << uri << " not found)\n";
        preload_writing();
    } catch (runtime_error& ex) {
        std::ostream out(&outbuf_);
        out << "(Error: " << ex.code() << " (" << iso8601_time{std::chrono::system_clock::now()} << " 'COMMAND_ERROR) "
            << ex.what() << "\n";
        preload_writing();
    } catch (...) {
        std::ostream out(&outbuf_);
        out << "(Error: " << UNKNOWN_ERROR << " (" << iso8601_time{std::chrono::system_clock::now()}
            << " 'COMMAND_ERROR) Unknown error\n";
        preload_writing();
    }
}

} // namespace cli

} // namespace decof
