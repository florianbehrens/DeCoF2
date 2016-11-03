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

#include "event.h"

#include <client_context/object_visitor.h>

namespace decof
{

event::event(std::string name, node *parent, userlevel_t writelevel) :
    object(name, parent, Forbidden, writelevel)
{}

void event::accept(object_visitor *visitor)
{
    visitor->visit(this);
}

} // namespace decof
