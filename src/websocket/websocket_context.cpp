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

#include <boost/asio/buffers_iterator.hpp>

#include <decof/exceptions.h>
#include <decof/websocket/websocket_context.h>

#include "request.h"

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
    using boost::asio::buffers_begin;
    using boost::asio::buffers_end;

    if (error)
        // disconnect()?
        return;

    auto begin = buffers_begin(inbuf_.data());
    auto end = buffers_end(inbuf_.data());

    request r;
    r.parse(begin, end);

    /* And send a JSON-RPC response like the following:
     * {
     *     result: true,        (value in case of a successful get method)
     *     result: {},          (empty result on successful request)
     *     error: {             (only in error case)
     *         code: -32601,
     *         message: "Method not found",
     *     },
     *     id: 123
     * }
     */

    std::cout << ">>" << beast::to_string(inbuf_.data()) << "<<\n";
    inbuf_.consume(inbuf_.size());

    read_message();
}

} // namespace websocket

} // namespace decof
