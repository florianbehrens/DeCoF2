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

#include <boost/asio.hpp>

#include <beast/core/error.hpp>
#include <beast/core/multi_buffer.hpp>
#include <beast/websocket/stream.hpp>

#include <decof/client_context/client_context.h>
#include <decof/client_context/update_container.h>

namespace decof
{

namespace websocket
{

/**
 * @brief WebSocket client context.
 */
class websocket_context final : public client_context
{
public:
    using socket_type = boost::asio::ip::tcp::socket;

    /** Constructor.
     *
     * @tparam MAX_MSG_SIZE The permitted maximum size of a WebSocket message.
     *
     * @param socket Rvalue reference socket.
     * @param od Reference to the object dictionary.
     * @param userlevel The contexts default userlevel.
     */
    template<size_t MAX_MSG_SIZE = 1048576>
    explicit websocket_context(socket_type&& socket, object_dictionary& od, userlevel_t userlevel = Normal) :
        client_context(od, userlevel),
        stream_(std::move(socket)),
        inbuf_(MAX_MSG_SIZE),
        outbuf_(MAX_MSG_SIZE)
    {
        stream_.read_message_max(MAX_MSG_SIZE);
    }

    std::string connection_type() const final;
    std::string remote_endpoint() const final;
    void preload() final;

    /**
     * @brief Overload that preloads an already upgraded stream.
     *
     * @note Calling this function on a not yet upgraded connection results in
     * undefined behavior!
     */
    void preload_upgraded();

private:
    /**
     * @brief Read a Websocket message from the stream in the input buffer.
     *
     * Does nothing if a read operation is already in progress or an incoming
     * request is pending in the input buffer.
     */
    void async_read_message();

    /**
     * @brief Write a Websocket message from the output buffer to the stream.
     *
     * Does nothing if a write operation is already in progress.
     */
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

    /**
     * @brief Processes request message in inbuf_.
     */
    void process_request();

    /**
     * @brief Writes a response/indication for sending in the output buffer.
     */
    void preload_writing();

    /**
     * @brief Slot for parameter change notifications.
     *
     * @param uri The fully qualified name of the parameter which value is
     * updated.
     * @param any_value The new value wrapped in a @a boost::any type.
     */
    void notify(const std::string& uri, const boost::any& any_value);

    /**
     * @brief Shutdown client context.
     *
     * @param error The error as reason for the close.
     */
    void shutdown(const beast::error_code& error);

    beast::websocket::stream<socket_type> stream_;
    beast::multi_buffer inbuf_;
    beast::multi_buffer outbuf_;
    update_container pending_updates_;

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
     * @brief Indicates a pending response message in inbuf_.
     *
     * @note Don't read anything from the stream_ into inbuf_ if this member
     * is true!
     */
    bool response_pending_ = false;
};

} // namespace websocket

} // namespace decof

#endif // DECOF_WEBSOCKET_CONTEXT_H
