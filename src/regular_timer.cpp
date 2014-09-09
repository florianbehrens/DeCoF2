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

#include "regular_timer.h"

namespace decof
{

regular_timer::regular_timer(boost::asio::io_service& io_service, std::chrono::milliseconds interval, bool started)
  : interval_(interval),
    timer_(io_service)
{
    if (started == true)
        start();
}

void regular_timer::start()
{
    next_tick_ = std::chrono::steady_clock::now() + interval_;
    timer_.expires_at(next_tick_);
    timer_.async_wait(std::bind(&regular_timer::handler, this, std::placeholders::_1));
}

regular_timer::connection regular_timer::observe(regular_timer::slot_type slot)
{
    return signal_.connect(slot);
}

void regular_timer::handler(const boost::system::error_code &error)
{
    BOOST_ASSERT_MSG(!error, "Unexpected error in regular_timer::handler occurred");

    // Call connected slots
    signal_();

    // Restart timer
    do {
        next_tick_ += interval_;
    } while (next_tick_ < std::chrono::steady_clock::now());

    timer_.expires_at(next_tick_);
    timer_.async_wait(std::bind(&regular_timer::handler, this, std::placeholders::_1));
}

} // namespace decof
