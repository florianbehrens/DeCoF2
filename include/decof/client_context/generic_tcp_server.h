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

#ifndef DECOF_GENERIC_TCP_SERVER_H
#define DECOF_GENERIC_TCP_SERVER_H

#include "client_context.h"
#include <decof/object_dictionary.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <memory>

namespace decof {

template <typename Context>
class generic_tcp_server
{
  public:
    /// Note that the callback function takes ownership of the provided socket object.
    generic_tcp_server(
        object_dictionary&                    obj_dict,
        boost::asio::io_service::strand&      strand,
        const boost::asio::ip::tcp::endpoint& endpoint,
        userlevel_t                           userlevel = Normal)
      : object_dictionary_(obj_dict),
        userlevel_(userlevel),
        strand_(strand),
        acceptor_(strand.get_io_service(), endpoint),
        socket_(strand.get_io_service())
    {
    }

    /// Returns the listening port.
    /// This is identical to the port given in constructor if non-zero.
    unsigned short port() const
    {
        return acceptor_.local_endpoint().port();
    }

    void preload()
    {
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        async_accept();
    }

    void async_accept()
    {
        acceptor_.async_accept(
            socket_, strand_.wrap(std::bind(&generic_tcp_server::accept_handler, this, std::placeholders::_1)));
    }

  private:
    void accept_handler(boost::system::error_code error)
    {
        // Check whether the server was stopped by a signal before this completion
        // handler had a chance to run.
        if (!acceptor_.is_open())
            return;

        if (!error) {
            // Create and preload new client context
            auto context = std::make_shared<Context>(strand_, std::move(socket_), object_dictionary_, userlevel_);
            object_dictionary_.add_context(context);
            context->preload();
        } else
            throw std::runtime_error(std::string(__FUNCTION__) + " received error: " + error.message());

        async_accept();
    }

    object_dictionary& object_dictionary_;
    userlevel_t        userlevel_;

    boost::asio::io_service::strand& strand_;
    boost::asio::ip::tcp::acceptor   acceptor_;
    boost::asio::ip::tcp::socket     socket_;
};

} // namespace decof

#endif // DECOF_GENERIC_TCP_SERVER_H
