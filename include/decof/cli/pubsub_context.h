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

#include <string>

#include <boost/any.hpp>
#include <boost/asio.hpp>

#include <decof/userlevel.h>

#include <decof/cli/cli_context_base.h>

#include "update_container.h"

namespace decof
{

// Forward declaration(s)
class object_dictionary;

namespace cli
{

class pubsub_context : public cli_context_base
{
public:
    /** Constructor.
     * @param socket Rvalue reference socket.
     * @param od Reference to the object dictionary.
     * @param userlevel The contexts default userlevel. */
    explicit pubsub_context(boost::asio::ip::tcp::socket&& socket, object_dictionary& od, userlevel_t userlevel = Normal);

    std::string connection_type() const final;
    std::string remote_endpoint() const final;
    void preload() final;

private:
    /// Callback for read operations.
    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /// Callback for write operations.
    void write_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /** Boost.Signals2 slot function for parameter change notifications.
     * @param uri The fully qualified name of the parameter which value is
     * updated.
     * @param any_value The new value wrapped in a @a boost::any type. */
    void notify(std::string uri, const boost::any &any_value);

    /** Initiate chain of write operations for pending updates.
     * @note Does nothing in case of no pending updates or in case a write
     * operation is currently active. */
    void preload_writing();

    /// Closes the socket and delists client context from object dictionary.
    void close();

    /// Processes CLI requests.
    void process_request(std::string request);

    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;

    size_t socket_send_buf_size_;

    update_container pending_updates_;
    bool writing_active_ = false;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_PUBSUB_CONTEXT_H
