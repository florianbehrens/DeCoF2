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

#ifndef SCHEME_MONITOR_PROTOCOL_H
#define SCHEME_MONITOR_PROTOCOL_H

#include <boost/asio.hpp>

#include "client_proxy.h"
#include "tree_element.h"

using boost::asio::ip::tcp;

class scheme_monitor_protocol : public ClientProxy
{
public:
    explicit scheme_monitor_protocol(Server& server, const tcp::endpoint &endpoint);

    virtual void preload() override;

private:
    void accept_handler(boost::system::error_code ec);

    void read_next();

    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    void write_next(std::string str);

    void write_handler(const boost::system::error_code&, std::size_t);

    void notify(const std::string &uri, const boost::any &any_value) noexcept;

    boost::asio::io_service &io_service_;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;
};

#endif // SCHEME_MONITOR_PROTOCOL_H
