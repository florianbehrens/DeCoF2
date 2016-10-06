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

#include "pubsub_context.h"

#include <string>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "exceptions.h"
#include "encoder.h"
#include "object_dictionary.h"

namespace decof
{

namespace cli
{

pubsub_context::pubsub_context(boost::asio::ip::tcp::socket&& socket, object_dictionary& od, userlevel_t userlevel) :
    client_context(od, userlevel),
    socket_(std::move(socket))
{
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
    return boost::lexical_cast<std::string>(socket_.remote_endpoint());
}

void pubsub_context::preload()
{
    auto self(std::dynamic_pointer_cast<pubsub_context>(shared_from_this()));
    boost::asio::async_read_until(socket_, inbuf_, '\n',
                                  std::bind(&pubsub_context::read_handler, self,
                                            std::placeholders::_1, std::placeholders::_2));
}

void pubsub_context::read_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
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
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, str, boost::algorithm::is_space(), boost::algorithm::token_compress_on);

        try {
            if (tokens.size() <= 1)
                throw parse_error();

            std::string& command = tokens[0];
            std::string& uri     = tokens[1];

            // Lower-case first substring
            std::transform(command.begin(), command.end(), command.begin(), ::tolower);

            // Remove optional "'" from parameter name
            if (uri[0] == '\'')
                uri.erase(0, 1);

            // Prepend root node name if not present (for compatibility reasons to
            // 'classic' DeCoF)
            if (uri != object_dictionary_.name() && !boost::algorithm::starts_with(uri, object_dictionary_.name() + ":"))
                uri = object_dictionary_.name() + ":" + uri;

            if (command == "subscribe" || command == "add") {
                observe(uri, std::bind(&pubsub_context::notify, this, std::placeholders::_1, std::placeholders::_2));
            } else if (command == "unsubscribe" || command == "remove") {
                unobserve(uri);
            } else
                throw unknown_operation_error();
        } catch (runtime_error& ex) {
            std::ostream out(&outbuf_);
            out << "ERROR " << ex.code() << ": " << ex.what() << std::endl;
            preload_writing();
        }

        preload();
    } else
        close();
}

void pubsub_context::write_handler(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error) {
        writing_active_ = false;
        preload_writing();
    } else
        close();
}

void pubsub_context::notify(std::string uri, const boost::any &any_value)
{
    // Cut root node name (for compatibility reasons to 'classic' DeCoF)
    if (uri != object_dictionary_.name())
        uri.erase(0, object_dictionary_.name().size() + 1);

    pending_updates_.push(uri, any_value);
    preload_writing();
}

void pubsub_context::preload_writing()
{
    if (writing_active_)
        return;

    std::ostream out(&outbuf_);

    while (!pending_updates_.empty() && outbuf_.size() < socket_send_buf_size_) {
        update_container::key_type uri;
        update_container::time_point time;
        boost::any any_value;

        std::tie(uri, any_value, time) = pending_updates_.pop_front();

        // Get current time in textual representation
        const size_t max_length = 25;
        char time_str[max_length];
        auto now = std::chrono::system_clock::to_time_t(time);
        std::strftime(time_str, sizeof(time_str), "%FT%T.000Z", std::localtime(&now));

        out << "(" << time_str << " '" << uri << " ";
        encoder().encode_any(out, any_value);
        out << ")\n";
    }

    if (outbuf_.size() == 0)
        return;

    auto self(std::dynamic_pointer_cast<pubsub_context>(shared_from_this()));
    boost::asio::async_write(socket_, outbuf_,
                             std::bind(&pubsub_context::write_handler, self,
                                       std::placeholders::_1, std::placeholders::_2));
    writing_active_ = true;
}

void pubsub_context::close()
{
    if (!socket_.is_open())
        return;

    socket_.close();

    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto sptr = shared_from_this();
    object_dictionary_.remove_context(sptr);
}

} // namespace cli

} // namespace decof
