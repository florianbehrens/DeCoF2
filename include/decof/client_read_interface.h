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
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/signal_type.hpp>
#include <boost/signals2/dummy_mutex.hpp>

namespace decof
{

using boost::signals2::keywords::mutex_type;
using boost::signals2::dummy_mutex;

class client_read_interface
{
public:
    /** @brief The signal type for object value change notifications.
     *
     * The first argument contains the object URI with the default separator
     * ':'. The second argument contains the object value wrapped in a @a
     * boost::any.
     */
    typedef boost::signals2::signal_type<void (const std::string&, const boost::any&), mutex_type<dummy_mutex>>::type value_change_signal_t;

    /** @brief The slot type for object value change notifications.
     *
     * See also #value_change_signal_t.
     */
    typedef value_change_signal_t::slot_type value_change_slot_t;

    virtual ~client_read_interface() {}

    /// Provides the value as runtime-generic type.
    virtual boost::any any_value() = 0;

    /** @brief Observe parameter value.
     *
     * Is called whenever a client context requests a parameter to be observed
     * the first time. Every further observation request (without a prior
     * unobservation request) does not result in an invokation of this
     * function.
     *
     * @param slot Slot object to be invoked on parameter value changes.
     */
    virtual boost::signals2::scoped_connection observe(value_change_slot_t slot) = 0;

    /** @brief Unobserve parameter value.
     *
     * Is called whenever a client context terminates parameter observation.
     * This function is called after deletion of the connection object returned
     * by #observe.
     */
    virtual void unobserve() = 0;
};

} // namespace decof

#endif // DECOF_CLIENT_READ_INTERFACE_H
