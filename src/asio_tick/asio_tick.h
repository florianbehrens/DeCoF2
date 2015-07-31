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

#ifndef DECOF_ASIO_TICK_H
#define DECOF_ASIO_TICK_H

#include <chrono>

#include <boost/asio/io_service.hpp>
#include <boost/asio/steady_timer.hpp>

#include "client_context.h"

namespace decof
{

namespace asio_tick
{

class asio_tick_context : public decof::client_context
{
public:
    explicit asio_tick_context(object_dictionary& obj_dict,
                               std::shared_ptr<boost::asio::io_service> io_service,
                               std::chrono::milliseconds interval = std::chrono::milliseconds(100));

    void preload() override;

private:
    void tick_handler(const boost::system::error_code& error);

    std::shared_ptr<boost::asio::io_service> io_service_;
    boost::asio::steady_timer timer_;
    std::chrono::milliseconds interval_;
};

} // namespace asio_tick

} // namespace decof

#endif // DECOF_ASIO_TICK_H
