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

#ifndef DECOF_BASIC_CLIENT_CONTEXT_H
#define DECOF_BASIC_CLIENT_CONTEXT_H

#include <decof/types.h>
#include <decof/userlevel.h>
#include <string>

namespace decof {

class object;
class object_dictionary;
class object_visitor;

/**
 * @brief Base class for client contexts.
 *
 * A client context instance represents a client connection to the object
 * dictionary server.
 *
 * A basic_client_context provides all features needed for connection types
 * that do not provide parameter observation functions. Use the
 * client_context subclass for connection types with parameter observation
 * features.
 */
class basic_client_context
{
  public:
    explicit basic_client_context(object_dictionary& od, userlevel_t ul = Normal);
    virtual ~basic_client_context() = default;

    /// Get current client contexts userlevel.
    userlevel_t userlevel() const;

    /** @brief Get current client contexts effective userlevel.
     *
     * @return #decof::Normal if current userlevel is #decof::Readonly,
     * otherwise #userlevel(). */
    userlevel_t effective_userlevel() const;

    /**
     * @brief Set current userlevel.
     * @param ul New userlevel.
     * @throw invalid_userlevel_error if given userlevel is invalid.
     */
    void userlevel(userlevel_t ul);

    /// Returns the connection type, e.g., "tcp", "serial".
    virtual std::string connection_type() const;

    /**
     * @brief Returns the connections' remote endpoint.
     * @return The remote endpoint as string or an empty string on error.
     */
    virtual std::string remote_endpoint() const;

  protected:
    /**
     * @brief Sets the value of the given object if it is a writable parameter.
     *
     * For convenience the obj argument is allowed to be nullptr.
     *
     * @param obj Pointer to object or nullptr.
     * @param value The new parameter value.
     * @throws If obj does not point to a writable parameter.
     */
    void set_parameter(object* obj, const value_t& value);

    /**
     * @brief Gets the value of the given object if it is a readable parameter.
     *
     * For convenience the obj argument is allowed to be nullptr.
     *
     * @param obj Pointer to object or nullptr.
     * @return Parameter value if obj points to a readable parameter.
     * @throws If obj does not point to a readable parameter.
     */
    value_t get_parameter(const object* obj);

    /**
     * @brief Signal event.
     *
     * For convenience the obj argument is allowed to be nullptr.
     *
     * @param obj Pointer to object or nullptr.
     * @param separator Hierachy level separator charater used in URI.
     * @throws If obj does not point to an event object.
     */
    void signal_event(object* obj);

    /**
     * @brief Traverse object tree using visitor pattern.
     *
     * For convenience the obj argument is allowed to be nullptr.
     *
     * @param obj Root element to start browsing or nullptr.
     * @param visitor Pointer to visitor object.
     * @throws If obj does not point to a valid object.
     */
    void browse_object(object* obj, object_visitor* visitor);

    object_dictionary& object_dictionary_;

  private:
    userlevel_t userlevel_;
};

} // namespace decof

#endif // DECOF_BASIC_CLIENT_CONTEXT_H
