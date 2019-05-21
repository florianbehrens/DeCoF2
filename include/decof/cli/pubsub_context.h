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

#ifndef DECOF_CLI_PUBSUB_CONTEXT_H
#define DECOF_CLI_PUBSUB_CONTEXT_H

#include "update_container.h"
#include <decof/cli/cli_context_base.h>
#include <decof/types.h>
#include <decof/userlevel.h>
#include <boost/any.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>
#include <string>

namespace decof {

// Forward declaration(s)
class object_dictionary;

namespace cli {

class pubsub_context : public cli_context_base, public std::enable_shared_from_this<pubsub_context>
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
    explicit pubsub_context(strand_t& strand, socket_t&& socket, object_dictionary& od, userlevel_t userlevel = Normal);

    std::string connection_type() const final;
    std::string remote_endpoint() const final;
    void        preload() final;

  private:
    /// Callback for read operations.
    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /// Callback for write operations.
    void write_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /**
     * @brief Boost.Signals2 slot function for parameter change notifications.
     *
     * @param uri The fully qualified name of the parameter which value is
     * updated.
     * @param value The new value. */
    void notify(std::string uri, const value_t& value);

    /** Initiate chain of write operations for pending updates.
     * @note Does nothing in case of no pending updates or in case a write
     * operation is currently active. */
    void preload_writing();

    /// Closes the socket and delists client context from object dictionary.
    void close();

    /// Processes CLI requests.
    void process_request(std::string request);

    strand_t&              strand_;
    socket_t               socket_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;

    size_t socket_send_buf_size_;

    update_container pending_updates_;
    bool             writing_active_ = false;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_PUBSUB_CONTEXT_H
