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

tcp_connection::tcp_connection(boost::asio::ip::tcp::socket socket)
  : socket_(std::move(socket))
{
    assert(socket_.is_open());
    connect_signal();
}

std::string tcp_connection::type() const
{
    return std::string("tcp");
}

std::string tcp_connection::remote_endpoint() const
{
    return boost::lexical_cast<std::string>(socket_.remote_endpoint());
}

void tcp_connection::async_read_until(char delim)
{
    boost::asio::async_read_until(socket_, inbuf_, delim, std::bind(&tcp_connection::read_handler, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void tcp_connection::async_write(const std::string &str)
{
    std::ostream os(&outbuf_);
    os << str;

    boost::asio::async_write(socket_, outbuf_, std::bind(&tcp_connection::write_handler, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void tcp_connection::disconnect()
{
    socket_.close();
    disconnect_signal();
}

std::shared_ptr<connection> tcp_connection::create(boost::asio::ip::tcp::socket socket)
{
    return std::shared_ptr<connection>(new tcp_connection(std::move(socket)));
}

void tcp_connection::read_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error) {
        // First copy bytes_transferred bytes from the streambuf to a
        // std::string. This is one of the uglyest things in boost::asio and only
        // properly documented in stackoverflow:
        // http://stackoverflow.com/questions/877652/copy-a-streambufs-contents-to-a-string.
        boost::asio::streambuf::const_buffers_type buffer = inbuf_.data();
        std::string str(boost::asio::buffers_begin(buffer), boost::asio::buffers_begin(buffer) + bytes_transferred);
        inbuf_.consume(bytes_transferred);
        read_signal(str);
    } else if (error.value() == boost::asio::error::eof) {
        // Connection was closed by peer
        disconnect_signal();
    } else {
        // Because we don't know what to do else we just close the connection
        disconnect();
    }
}

void tcp_connection::write_handler(const boost::system::error_code &error, std::size_t)
{
    if (error) {
        // Because we don't know what to do else we just close the connection
        disconnect();
    }
}

} // namespace decof
