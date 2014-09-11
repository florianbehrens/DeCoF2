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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#define signals

namespace decof
{

class connection
{
public:
    virtual ~connection() {}

    virtual std::string type() const = 0;
    virtual std::string remote_endpoint() const = 0;

    virtual void async_read_until(char delim) = 0;
    virtual void async_write(const std::string&) = 0;

    virtual void disconnect() = 0;

public signals:
    boost::signals2::signal<void ()> connect_signal;
    boost::signals2::signal<void ()> disconnect_signal;
    boost::signals2::signal<void (const std::string& str)> read_signal;
    boost::signals2::signal<void ()> write_signal;
};

} // namespace decof

#endif // CONNECTION_H
