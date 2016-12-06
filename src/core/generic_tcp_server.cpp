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

#include <client_context/generic_tcp_server.h>

using boost::asio::ip::tcp;

namespace decof
{

generic_tcp_server::generic_tcp_server(object_dictionary& obj_dict, std::shared_ptr<boost::asio::io_service> io_service, const tcp::endpoint& endpoint, userlevel_t userlevel)
  : object_dictionary_(obj_dict),
    acceptor_(*io_service.get(), endpoint),
    socket_(*io_service.get()),
    userlevel_(userlevel)
{}

unsigned short generic_tcp_server::port() const
{
    return acceptor_.local_endpoint().port();
}

} // namespace decof
