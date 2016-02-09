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

#include "asio_tick.h"

namespace decof
{

namespace asio_tick
{

asio_tick_context::asio_tick_context(decof::object_dictionary &obj_dict, std::shared_ptr<boost::asio::io_service> io_service, std::chrono::milliseconds interval) :
    client_context(obj_dict),
    io_service_(io_service),
    timer_(*io_service),
    interval_(interval)
{}

void asio_tick_context::preload()
{
    timer_.expires_from_now(interval_);
    timer_.async_wait(std::bind(&asio_tick_context::tick_handler, this, std::placeholders::_1));
}

void asio_tick_context::tick_handler(const boost::system::error_code &error)
{
    if (error != boost::asio::error::operation_aborted) {
        tick();
        preload();
    }
}

} // namespace asio_tick

} // namespace decof
