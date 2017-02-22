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

#include <boost/asio.hpp>

#include <decof/client_context/client_context.h>

namespace decof
{

namespace websocket
{

class websocket_context final : public client_context
{
public:
    /** Constructor.
     * @param socket Rvalue reference socket.
     * @param od Reference to the object dictionary.
     * @param userlevel The contexts default userlevel. */
    explicit websocket_context(boost::asio::ip::tcp::socket&& socket, object_dictionary& od, userlevel_t userlevel = Normal);

    std::string connection_type() const final;
    std::string remote_endpoint() const final;
    void preload() final;

private:
    /** @brief Callback for boost::asio read operations.
     *
     * Parses and evaluates SCGI requests. */
    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /// Handle HTTP GET request.
    /// A GET request is used to read readable parameters.
    void handle_get_request();

    /// Handle HTTP PUT request.
    /// A PUT request is used to modify readwrite parameters. According to the
    /// HTTP/1.1 specification (RFC2616, clause §9.2.1), PUT requests shall be
    /// idempotent (i.e., multiple invocations result in the same side effects
    /// as a single invocation).
    void handle_put_request();

    /// Handle HTTP POST request.
    /// POST requests must be used to modify writeonly parameters or to execute
    /// events.
    void handle_post_request();

    /// Sends the given reply to the client.
    void send_response(const response &resp);

    /// Callback for boost::asio write operations.
    void write_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /// Closes the socket and delists client context from object dictionary.
    void disconnect();

    boost::asio::ip::tcp::socket socket_;

    static const size_t inbuf_size_ = 1500;
    std::array<char, inbuf_size_> inbuf_;
    boost::asio::streambuf outbuf_;

    /// The remote endpoint (HTTP client) as taken from the SCGI request.
    std::string remote_endpoint_;

    request_parser parser_;
};

} // namespace websocket

} // namespace decof

#endif // DECOF_WEBSOCKET_CONTEXT_H
