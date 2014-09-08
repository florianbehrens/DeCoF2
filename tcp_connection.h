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

#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include "connection.h"

#include <memory>

#include <boost/asio.hpp>

namespace decof
{

class tcp_connection : public connection, public std::enable_shared_from_this<connection>
{
private:
    explicit tcp_connection(boost::asio::ip::tcp::socket socket);

public:
    virtual ~tcp_connection();

    virtual std::string type() const override;
    virtual std::string remote_endpoint() const override;

    virtual void async_read_until(char delim) override;

    virtual void async_write(const std::string& str) override;

    static std::shared_ptr<connection> create(boost::asio::ip::tcp::socket socket);

private:
    /// Callback for boost::asio read operations.
    void read_handler(/*std::shared_ptr<tcp_connection>, */const boost::system::error_code& error, std::size_t bytes_transferred);

    /// Callback for boost::asio write operations.
    void write_handler(/*std::shared_ptr<tcp_connection>, */const boost::system::error_code& error, std::size_t bytes_transferred);

    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;
};

} // namespace decof

#endif // TCP_CONNECTION_H
