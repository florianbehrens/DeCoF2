/*
 * Copyright (c) 2015 Florian Behrens
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

#ifndef DECOF_SCGI_CONTEXT_H
#define DECOF_SCGI_CONTEXT_H

#include "request_parser.h"
#include "response.h"
#include <decof/client_context/client_context.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <array>
#include <memory>
#include <string>

namespace decof {

namespace scgi {

class scgi_context final : public client_context, public std::enable_shared_from_this<scgi_context>
{
  public:
    using strand_t = boost::asio::io_service::strand;
    using socket_t = boost::asio::ip::tcp::socket;

    /** @brief Constructor.
     *
     * @param strand Reference to a Boost.Asio strand object used to dispatch
     * the handlers.
     * @param socket Rvalue reference socket.
     * @param od Reference to the object dictionary.
     * @param userlevel The contexts default userlevel.
     */
    explicit scgi_context(strand_t& strand, socket_t&& socket, object_dictionary& od, userlevel_t userlevel = Normal);

    std::string connection_type() const final;
    std::string remote_endpoint() const final;

    void preload();

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
    void send_response(const response& resp);

    /// Callback for boost::asio write operations.
    void write_handler(const boost::system::error_code& error, std::size_t);

    /// Closes the socket and delists client context from object dictionary.
    void disconnect();

    strand_t& strand_;
    socket_t  socket_;

    static const size_t           inbuf_size_ = 1500;
    std::array<char, inbuf_size_> inbuf_;
    boost::asio::streambuf        outbuf_;

    /// The remote endpoint (HTTP client) as taken from the SCGI request.
    std::string remote_endpoint_;

    request_parser parser_;
};

} // namespace scgi

} // namespace decof

#endif // DECOF_SCGI_CONTEXT_H
