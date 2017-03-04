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
    /// Read Websocket message from stream.
    void async_read_message();

    /// Write pending response to stream.
    void async_write_message();

    /**
     * @brief Read handler.
     *
     * This handler is called whenever a Websocket message was received.
     *
     * @param error Error code.
     */
    void read_handler(const beast::error_code& error);

    /**
     * @brief Write handler.
     *
     * This handler is called whenever a Websocket message was sent.
     *
     * @param error Error code.
     */
    void write_handler(const beast::error_code& error);

    /// Processes request message in inbuf_.
    void process_request();

    /**
     * @brief Slot for parameter change notifications.
     *
     * @param uri The fully qualified name of the parameter which value is
     * updated.
     * @param any_value The new value wrapped in a @a boost::any type.
     */
    void notify(const std::string& uri, const boost::any& any_value);

    /// Shutdown client context.
    void shutdown();

    beast::websocket::stream<socket_type> stream_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;

    /**
     * @brief Indicates a read operation in progress.
     *
     * @note Don't read anything from the stream_ if this member is true!
     */
    bool reading_active_ = false;

    /**
     * @brief Indicates a write operation in progress.
     *
     * @note Don't write anything on the stream_ if this member is true!
     */
    bool writing_active_ = false;

    /**
     * @brief Indicates a pending response message in outbuf_.
     *
     * @note Don't read anything from the stream_ into outbuf_ if this member
     * is true!
     */
    bool response_pending_ = false;
};

} // namespace websocket

} // namespace decof

#endif // DECOF_WEBSOCKET_CONTEXT_H
