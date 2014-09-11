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

#ifndef DECOF_EVENT_H
#define DECOF_EVENT_H

#include "tree_element.h"

// Macro for simple event declaration.
#define DECOF_DECLARE_EVENT(name)                                             \
    struct name : public decof::event                                         \
    {                                                                         \
        using event::event;                                                   \
        virtual void signal() override final;                                 \
    }

namespace decof
{

class event : public tree_element
{
public:
    explicit event(std::string name, node *parent = nullptr, userlevel_t writelevel = Normal);
    virtual void signal() = 0;
};

} // namespace decof

#endif // DECOF_EVENT_H