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

#ifndef DECOF_OBJECT_DICTIONARY_H
#define DECOF_OBJECT_DICTIONARY_H

#include "node.h"
#include <list>
#include <string_view>

namespace decof {

class basic_client_context;
struct tick_interface;

/**
 * @brief Dictionary of tree objects.
 *
 * @note Make sure the lifetime of the object dictionary exceeds the lifetime
 * of all associated parameters, expecially those that are registered as tick
 * consumers!
 */
class object_dictionary : public node
{
    friend class client_context;

  public:
    class context_guard
    {
      public:
        context_guard(object_dictionary& od, basic_client_context* cc);
        ~context_guard();

        context_guard(const context_guard&) = delete;
        context_guard& operator=(const context_guard&) = delete;

      private:
        object_dictionary&          object_dictionary_;
        const basic_client_context* client_context_;
    };

    object_dictionary(const char* root_uri = "root");

    const basic_client_context* current_context() const;

    /**
     * @brief Register a tick target at the tick source.
     *
     * @param tick_target The tick target object.
     */
    void register_for_tick(tick_interface* tick_target);

    /**
     * @brief Unregister a tick target at the tick source.
     *
     * @pre The tick_target must have be registered before.
     * @param tick_target The tick target object.
     */
    void unregister_for_tick(tick_interface* tick_target);

    /**
     * @brief Find object with given URI.
     *
     * Returns a pointer to the object corresponding to the given URI and
     * separator if existing, otherwise @c nullptr.
     *
     * A leading separator character is optional.
     *
     * @param uri The URI to the requested object.
     * @param separator The separator character used with the URI.
     * @return Pointer to the requested object if existing or @c nullptr.
     */
    object* find_object(std::string_view uri, char separator = ':');

  private:
    void set_current_context(basic_client_context* client_context);

    void tick();

    basic_client_context*      current_context_{nullptr};
    std::list<tick_interface*> tick_targets_;
};

} // namespace decof

#endif // DECOF_OBJECT_DICTIONARY_H
