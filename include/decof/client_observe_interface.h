/*
 * Copyright (c) 2019 Florian Behrens
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

#ifndef DECOF_CLIENT_OBSERVE_INTERFACE_H
#define DECOF_CLIENT_OBSERVE_INTERFACE_H

#include "types.h"
#include <boost/signals2/connection.hpp>
#include <boost/signals2/dummy_mutex.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/signals2/signal_type.hpp>
#include <string>

namespace decof {

/**
 * @brief The signal type for parameter value change notifications.
 *
 * The first argument contains the object URI with the default separator
 * ':'. The second argument contains the objects value.
 */
using value_change_signal = boost::signals2::signal_type<
    void(const std::string&, const value_t&),
    boost::signals2::keywords::mutex_type<boost::signals2::dummy_mutex>>::type;

/// The slot type for parameter value change notifications.
using value_change_slot = value_change_signal::slot_type;

/**
 * @brief Interface for client observe access to parameter value.
 */
struct client_observe_interface
{
    virtual ~client_observe_interface() = default;

    /**
     * @brief Register slot for parameter value observation.
     * @param slot Slot object to be invoked on parameter value changes.
     */
    virtual boost::signals2::scoped_connection observe(value_change_slot slot) = 0;

    /**
     * @brief Unregister parameter observation slot.
     *
     * @note This function must be called after deletion of the connection
     * object returned observe.
     */
    virtual void unobserve() = 0;
};

} // namespace decof

#endif // DECOF_CLIENT_OBSERVE_INTERFACE_H
