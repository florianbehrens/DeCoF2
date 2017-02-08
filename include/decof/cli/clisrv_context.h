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

#ifndef DECOF_CLI_CLISRV_CONTEXT_H
#define DECOF_CLI_CLISRV_CONTEXT_H

#include <functional>
#include <string>

#include <boost/asio.hpp>

#include <decof/cli/cli_context_base.h>

namespace decof
{

namespace cli
{

class clisrv_context : public cli_context_base
{
public:
    /** Constructor.
     * @param socket Rvalue reference socket.
     * @param od Reference to the object dictionary.
     * @param userlevel The contexts default userlevel. */
    explicit clisrv_context(boost::asio::ip::tcp::socket&& socket, object_dictionary& od, userlevel_t userlevel = Normal);

    virtual std::string connection_type() const final;
    virtual std::string remote_endpoint() const final;
    virtual void preload() final;

private:
    /// Callback for boost::asio write operations.
    void write_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /** @brief Callback for boost::asio read operations.
     *
     * Parses and evaluates CLI client/server command lines. Expects
     * command lines like: <operation> [ <uri> [ <value-string> ]].
     * Operation must be one of: get, param-ref, set, param-set!, signal, exec,
     * browse, param-disp. */
    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    /// Closes the socket and delists client context from object dictionary.
    void disconnect();

    /// Processes CLI requests.
    void process_request(std::string request);

    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_CLISRV_CONTEXT_H
