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

#include "tcp_connection.h"

#include <boost/lexical_cast.hpp>

namespace decof
{

tcp_connection::tcp_connection(std::shared_ptr<boost::asio::ip::tcp::socket> a_socket)
  : socket_(a_socket)
{}

std::string tcp_connection::endpoint() const
{
    return boost::lexical_cast<std::string>(socket_->remote_endpoint());
}

void tcp_connection::async_read_until(boost::asio::streambuf &, char delim)
{}

void tcp_connection::async_write(const std::string &response)
{}

} // namespace decof
