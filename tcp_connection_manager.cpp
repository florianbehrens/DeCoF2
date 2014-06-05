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

#include "tcp_connection_manager.h"

#include "object_dictionary.h"

using boost::asio::ip::tcp;

decof::tcp_connection_manager::tcp_connection_manager(object_dictionary& object_dictionary, const tcp::endpoint& endpoint, decof::tcp_connection_manager::callback callback)
  : object_dictionary_(object_dictionary),
    acceptor_(object_dictionary.get_io_service(), endpoint),
    callback_(callback)
{}

void decof::tcp_connection_manager::preload()
{
    // Create new socket
    socket_.reset(new tcp::socket(object_dictionary_.get_io_service()));
    acceptor_.async_accept(
        *socket_,
        std::bind(&tcp_connection_manager::accept_handler, this, std::placeholders::_1)
    );
}

void decof::tcp_connection_manager::accept_handler(boost::system::error_code error)
{
    // Check whether the server was stopped by a signal before this completion
    // handler had a chance to run.
    if (!acceptor_.is_open())
        return;

    if (!error) {
        // Forward socket to registered component
        callback_(object_dictionary_, std::move(socket_));
        preload();
    } else
        throw std::runtime_error(error.message());
}
