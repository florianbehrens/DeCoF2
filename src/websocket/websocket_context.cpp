/*
 * Copyright (c) 2017 Florian Behrens
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

// TODO:
#include <iostream>

#include <beast/core/to_string.hpp>
#include <beast/websocket/option.hpp>

#include <decof/websocket/websocket_context.h>

namespace decof
{

namespace websocket
{

websocket_context::websocket_context(websocket_context::socket_type&& socket, decof::object_dictionary& od, decof::userlevel_t userlevel) :
    client_context(od, userlevel),
    stream_(std::move(socket))
{
    stream_.set_option(beast::websocket::read_message_max(1e6));
}

std::string websocket_context::connection_type() const
{
    return "websocket";
}

std::string websocket_context::remote_endpoint() const
{
    return remote_endpoint_;
}

void websocket_context::preload()
{
    auto self(std::dynamic_pointer_cast<websocket_context>(shared_from_this()));
    stream_.async_accept([self](const beast::error_code &error) {
        if (error)
            // disconnect()?
            return;

        self->read_message();
    });
}

void websocket_context::read_message()
{
    beast::websocket::opcode opcode;
    auto self(std::dynamic_pointer_cast<websocket_context>(shared_from_this()));

    stream_.async_read(opcode, inbuf_, [self](const beast::error_code &error) { self->read_handler(error); });
}

void websocket_context::read_handler(const beast::error_code &error)
{
    if (error)
        // disconnect()?
        return;

    /* Parse a JSON request like the following:
     * {
     *     request: "get", // or set, sub[scribe], unsub[scribe]
     *     path: "laser1:enable",
     *     value: true  // Only when method == set
     * }
     */

    /* And send a JSON response like the following:
     * {
     *     response: "",
     *     uri: "laser1:enable",
     *     value: true
     * }
     */

    std::cout << ">>" << beast::to_string(inbuf_.data()) << "<<\n";
    inbuf_.consume(inbuf_.size());

    read_message();
}

} // namespace websocket

} // namespace decof
