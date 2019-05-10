/*
 * Copyright (c) 2015 Florian Behrens
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

#ifndef DECOF_HANDLER_EVENT_H
#define DECOF_HANDLER_EVENT_H

#include "event.h"
#include <functional>

namespace decof
{

/** @brief Handler-based event class.
 *
 * This class is the handler version of class #event. It supports runtime
 * registration of callable(s) as handler function(s).
 */
class handler_event : public event
{
public:
    explicit handler_event(std::string name, node *parent = nullptr,
                           std::function<void()> signal_handler = nullptr,
                           userlevel_t writelevel = Normal);

    /// @brief Set signal event handler.
    /// @param handler Handler for event signal.
    /// @note If a default constructed object of type @a std::function is given,
    /// the invocation of the respective signal is silently ignored.
    void signal_handler(std::function<void()> handler);

private:
    virtual void signal() override final;

    std::function<void()> signal_handler_;
};

} // namespace decof

#endif // DECOF_HANDLER_EVENT_H
