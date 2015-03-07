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

#ifndef DECOF_TCP_CONNECTION_MANAGER_H
#define DECOF_TCP_CONNECTION_MANAGER_H

#include <memory>

#include <boost/asio.hpp>

#include "client_context.h"
#include "object_dictionary.h"
#include "tcp_connection.h"

namespace decof
{

class tcp_connection_manager
{
public:
    /// Note that the callback function takes ownership of the provided socket object.
    tcp_connection_manager(object_dictionary& a_object_dictionary, const boost::asio::ip::tcp::endpoint& endpoint, userlevel_t userlevel = Normal);

    /// Returns the listening port.
    /// This is identical to the port given in constructor if non-zero.
    unsigned short port() const;

    template<typename CTX>
    void preload()
    {
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        async_accept<CTX>();
    }

    template<typename CTX>
    void async_accept()
    {
        acceptor_.async_accept(socket_, std::bind(&tcp_connection_manager::accept_handler<CTX>, this, std::placeholders::_1));
    }

private:
    template<typename CTX>
    void accept_handler(boost::system::error_code error)
    {
        // Check whether the server was stopped by a signal before this completion
        // handler had a chance to run.
        if (!acceptor_.is_open())
            return;

        if (!error) {
            // Create and preload new client context
            std::shared_ptr<client_context> cli_ctx(new CTX(object_dictionary_, tcp_connection::create(std::move(socket_)), userlevel_));
            object_dictionary_.add_context(cli_ctx);
            cli_ctx->preload();
        } else
            throw std::runtime_error(std::string(__FUNCTION__) + " received error: " + error.message());

        async_accept<CTX>();
    }

    object_dictionary& object_dictionary_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    userlevel_t userlevel_;
};

} // namespace decof

#endif // DECOF_TCP_CONNECTION_MANAGER_H
