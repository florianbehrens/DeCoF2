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

#include <decof/client_context/client_context.h>
#include <decof/client_write_interface.h>
#include <decof/event.h>
#include <decof/exceptions.h>
#include <decof/object.h>
#include <decof/object_dictionary.h>

namespace decof {

client_context::client_context(object_dictionary& od, userlevel_t ul) : basic_client_context(od, ul)
{
}

void client_context::observe(object* obj, value_change_slot slot)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    auto observable = dynamic_cast<client_observe_interface*>(obj);
    if (observable == nullptr)
        throw invalid_parameter_error();
    if (effective_userlevel() > obj->readlevel())
        throw access_denied_error();

    auto uri = obj->fq_name();
    if (observables_.count(uri) == 0) {
        observables_[uri] = observable->observe(slot);
    } else {
        if (auto readable = dynamic_cast<const client_read_interface*>(obj)) {
            // TODO: Raise error or deliver value?
            slot(uri, readable->generic_value());
        }
    }
}

void client_context::unobserve(object* obj)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    auto uri = obj->fq_name();

    if (observables_.count(uri) == 0)
        throw not_subscribed_error();

    auto observable = dynamic_cast<client_observe_interface*>(obj);
    if (observable == nullptr)
        throw invalid_parameter_error();

    observables_.erase(uri);
    observable->unobserve();
}

void client_context::tick()
{
    object_dictionary_.tick();
}

} // namespace decof
