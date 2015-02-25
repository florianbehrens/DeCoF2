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

#ifndef DECOF_CONNECTION_H
#define DECOF_CONNECTION_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#define signals

namespace decof
{

/** Base class for all transport connections.
 *
 * This class provides basic functionality for all different transport
 * connections such as TCP, serial line, etc.
 *
 * Though heavily based on @a boost::asio which uses compile-time
 * polymorphism with templates, this class uses runtime polymorphism with
 * virtual functions. This makes the implementation of transport protocols
 * easier because there is no need for heavy template programming.
 *
 * For reading and writing over the connection virtual functions are provided
 * in a similar manner as @a boost::asio provides. Callbacks are implemented
 * based on @a boost::signals2.
 */
class connection
{
public:
    virtual ~connection() {}

    virtual std::string type() const = 0;
    virtual std::string remote_endpoint() const = 0;

    virtual void async_read_until(char) = 0;
    virtual void async_read_some() = 0;
    virtual void async_write(const std::string&) = 0;

    virtual void disconnect() = 0;

public signals:
    boost::signals2::signal<void ()> connect_signal;
    boost::signals2::signal<void ()> disconnect_signal;
    boost::signals2::signal<void (const std::string& str)> read_signal;
    boost::signals2::signal<void ()> write_signal;
};

} // namespace decof

#endif // DECOF_CONNECTION_H
