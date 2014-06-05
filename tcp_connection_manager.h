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

#ifndef TCP_CONNECTION_MANAGER_H
#define TCP_CONNECTION_MANAGER_H

#include <memory>

#include <boost/asio.hpp>

namespace decof
{

// Forward declarations
class object_dictionary;

class tcp_connection_manager
{
public:
    typedef void (*callback)(object_dictionary& object_dictionary, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    /// Note that the callback function takes ownership of the provided socket object.
    tcp_connection_manager(object_dictionary& object_dictionary, const boost::asio::ip::tcp::endpoint& endpoint, callback callback);

    void preload();

private:
    void accept_handler(boost::system::error_code error);

    object_dictionary& object_dictionary_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
    callback callback_;
};

} // namespace decof

#endif // TCP_CONNECTION_MANAGER_H
