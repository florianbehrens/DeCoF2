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

#ifndef DECOF_WEBSOCKET_CONTEXT_H
#define DECOF_WEBSOCKET_CONTEXT_H

#include <array>
#include <string>
#include <system_error>

#include <beast/core/error.hpp>
#include <beast/core/streambuf.hpp>
#include <beast/websocket/stream.hpp>
#include <boost/asio.hpp>

#include <decof/client_context/client_context.h>

namespace decof
{

namespace websocket
{

class websocket_context final : public client_context
{
public:
    using socket_type = boost::asio::ip::tcp::socket;

    /** Constructor.
     *
     * @param socket Rvalue reference socket.
     * @param od Reference to the object dictionary.
     * @param userlevel The contexts default userlevel.
     */
    explicit websocket_context(socket_type&& socket, object_dictionary& od, userlevel_t userlevel = Normal);

    std::string connection_type() const final;
    std::string remote_endpoint() const final;
    void preload() final;

private:
    /// @brief Read Websocket message from stream.
    void read_message();

    /** @brief Read handler.
     *
     * This handler is called whenever a Websocket message was received.
     *
     * @param error Error code.
     */
    void read_handler(const beast::error_code& error);

    beast::websocket::stream<socket_type> stream_;
    beast::streambuf inbuf_;

    std::string remote_endpoint_;
};

} // namespace websocket

} // namespace decof

#endif // DECOF_WEBSOCKET_CONTEXT_H
