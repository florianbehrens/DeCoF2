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

#include "handler_event.h"

namespace decof {

handler_event::handler_event(
    std::string name, node* parent, std::function<void()> signal_handler, userlevel_t writelevel)
  : event(name, parent, writelevel), signal_handler_(signal_handler)
{
}

void handler_event::signal_handler(std::function<void()> handler)
{
    signal_handler_ = handler;
}

void handler_event::signal()
{
    if (signal_handler_)
        signal_handler_();
}

} // namespace decof
