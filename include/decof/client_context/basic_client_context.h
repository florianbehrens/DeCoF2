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
    void    set_parameter(const std::string& uri, const value_t& value, char separator = ':');
    value_t get_parameter(const std::string& uri, char separator = ':');

    /// Signal event.
    /// @param uri The event URI.
    /// @param separator Hierachy level separator charater used in URI.
    void signal_event(const std::string& uri, char separator = ':');

    /** @brief Traverse object tree using visitor pattern.
     * @param visitor Pointer to visitor object.
     * @param root_uri Root element to start browsing. If omitted browsing
     * starts at root uri. */
    void browse(object_visitor* visitor, const std::string& root_uri = std::string());

    object_dictionary& object_dictionary_;

  private:
    void browse_object(object* te, object_visitor* visitor);

    userlevel_t userlevel_;
};

} // namespace decof

#endif // DECOF_BASIC_CLIENT_CONTEXT_H
