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

#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>

#include "object_dictionary.h"
#include "poll_timer.h"

namespace decof
{

class server
{
public:
    object_dictionary& objectDictionary();
    poll_timer& pollTimer();
    boost::asio::io_service& ioService();

private:
    object_dictionary objectDictionary_;
    poll_timer pollTimer_;
    boost::asio::io_service ioService_;
};

} // namespace decof

#endif // SERVER_H
