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

#ifndef REGULAR_TIMER_H
#define REGULAR_TIMER_H

#include <chrono>

#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/signals2.hpp>

namespace decof
{

/// @brief The regular_timer class represents a regular timer.
/// The implementation is based on boost::asio timers.
class regular_timer
{
    friend class object_dictionary;

public:
    typedef boost::signals2::signal<void ()> signal_type;
    typedef signal_type::slot_type slot_type;
    typedef boost::signals2::connection connection;

    /// Registers an timer observer.
    connection observe(slot_type slot);

private:
    explicit regular_timer(boost::asio::io_service& io_service, std::chrono::milliseconds interval, bool started = false);

    /// Starts the timer.
    void start();

    void handler(const boost::system::error_code& error);

    std::chrono::milliseconds interval_;
    std::chrono::steady_clock::time_point next_tick_;
    boost::asio::steady_timer timer_;
    signal_type signal_;
};

} // namespace decof

#endif // REGULAR_TIMER_H
