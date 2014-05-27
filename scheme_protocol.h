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

#ifndef SCHEME_PROTOCOL_H
#define SCHEME_PROTOCOL_H

#include <array>
#include <string>

#include <boost/asio.hpp>

#include "client_proxy.h"

using boost::asio::ip::tcp;

class scheme_protocol : public ClientProxy
{
public:
    explicit scheme_protocol(Server& server, const tcp::endpoint &endpoint);

    virtual void preload() override;

private:
    void accept_handler(const boost::system::error_code &error);

    void read_next();

    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    void write_next(std::string str);

    void write_handler(const boost::system::error_code& error, std::size_t);

    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;
};

#endif // SCHEME_PROTOCOL_H
