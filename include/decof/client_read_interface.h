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

#ifndef DECOF_CLIENT_READ_INTERFACE_H
#define DECOF_CLIENT_READ_INTERFACE_H

#include <boost/any.hpp>
#include <boost/signals2.hpp>

namespace decof
{

class client_read_interface
{
public:
    typedef boost::signals2::signal<void (const std::string&, const boost::any&)> signal_type;
    typedef signal_type::slot_type slot_type;

    virtual ~client_read_interface() {}

    // Provides the value as runtime-generic type.
    virtual boost::any any_value() = 0;

    // Observe parameter value.
    virtual boost::signals2::connection observe(slot_type slot) = 0;
};

} // namespace decof

#endif // DECOF_CLIENT_READ_INTERFACE_H
