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

class tcp_connection : public connection
{
    friend class tcp_connection_manager;

private:
    explicit tcp_connection(std::shared_ptr<boost::asio::ip::tcp::socket> a_socket);

    virtual std::string endpoint() const override;

    virtual void async_read_until(boost::asio::streambuf&, char delim) override;

    virtual void async_write(const std::string& response) override;

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
};

} // namespace decof

#endif // TCP_CONNECTION_H
