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

using boost::asio::ip::tcp;

namespace decof
{

tcp_connection_manager::tcp_connection_manager(object_dictionary& a_object_dictionary, const tcp::endpoint& endpoint)
  : object_dictionary_(a_object_dictionary),
    acceptor_(a_object_dictionary.get_io_service(), endpoint)
{}

} // namespace decof
