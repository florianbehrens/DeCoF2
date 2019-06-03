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

#ifndef DECOF_CLIENT_CONTEXT_H
#define DECOF_CLIENT_CONTEXT_H

#include <decof/client_context/basic_client_context.h>
#include <decof/client_observe_interface.h>
#include <decof/userlevel.h>
#include <boost/signals2/connection.hpp>
#include <map>
#include <string>

namespace decof {

/**
 * @brief Base class for advanced client contexts.
 *
 * A client context instance represents a client connection to the object
 * dictionary server.
 */
class client_context : public basic_client_context
{
  public:
    explicit client_context(object_dictionary& od, userlevel_t ul = Normal);
    virtual ~client_context() = default;

  protected:
    /**
     * @brief Observe parameter object.
     *
     * For convenience the obj argument is allowed to be nullptr.
     *
     * @param obj Pointer to object or nullptr.
     * @param slot The slot to be called on object updates.
     * @throws If obj does not point to an observable parameter.
     */
    void observe(object* obj, value_change_slot slot);

    /**
     * @brief Terminate parameter object observation.
     *
     * For convenience the obj argument is allowed to be nullptr.
     *
     * @param obj Pointer to object or nullptr.
     * @throws If obj does not point to an observable parameter of parameter
     * object wasn't subscribed.
     */
    void unobserve(object* obj);

    /// @brief Timer tick.
    /// Call this member function regularly in order to check for value changes
    /// of observed external_readonly_parameters.
    void tick();

  private:
    std::map<std::string, boost::signals2::scoped_connection> observables_;
};

} // namespace decof

#endif // DECOF_CLIENT_CONTEXT_H
