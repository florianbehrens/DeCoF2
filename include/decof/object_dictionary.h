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
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <memory>

namespace decof {

class client_context;

/// @brief Object dictionary for parameter tree objects.
class object_dictionary : public node
{
    friend class client_context;

  public:
    typedef boost::signals2::signal<void()>    tick_type;
    typedef tick_type::slot_type               tick_slot_type;
    typedef boost::signals2::scoped_connection tick_connection;

    class context_guard
    {
      public:
        context_guard(object_dictionary& od, client_context* cc);
        ~context_guard();

        context_guard(const context_guard&) = delete;
        context_guard& operator=(const context_guard&) = delete;

      private:
        object_dictionary&    object_dictionary_;
        const client_context* client_context_;
    };

    object_dictionary(const std::string& root_uri = "root");

    void                                  add_context(std::shared_ptr<client_context> client_context);
    void                                  remove_context(std::shared_ptr<client_context> client_context);
    const std::shared_ptr<client_context> current_context() const;

    /// @brief Registers a timer observer.
    /// The connection with the timer registrar is based on @a boost::signals2.
    /// @param slot The signal slot.
    /// @return A @a boost::signals2 connection object.
    tick_connection register_for_tick(tick_slot_type slot);

  private:
    object* find_object(const std::string& curi, char separator = ':');

    void set_current_context(client_context* client_context);

    void tick();

    std::list<std::shared_ptr<client_context>> client_contexts_;
    std::shared_ptr<client_context>            current_context_;
    tick_type                                  tick_signal_;
};

} // namespace decof

#endif // DECOF_OBJECT_DICTIONARY_H
