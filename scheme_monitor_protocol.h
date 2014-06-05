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

#include <memory>

#include <boost/asio.hpp>

#include "client_proxy.h"
#include "tcp_connection_manager.h"

namespace decof
{

class scheme_monitor_protocol : public client_proxy
{
public:
    explicit scheme_monitor_protocol(object_dictionary& object_dictionary, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    static void handle_connect(object_dictionary& object_dictionary, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

private:
    void read_next();
    void read_handler(const boost::system::error_code& error, std::size_t bytes_transferred);

    void write_next(std::string str);
    void write_handler(const boost::system::error_code&, std::size_t);

    void notify(const std::string &uri, const boost::any &any_value) noexcept;

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    boost::asio::streambuf inbuf_;
    boost::asio::streambuf outbuf_;
};

} // namespace decof

#endif // SCHEME_MONITOR_PROTOCOL_H
