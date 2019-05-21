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

#include "object_dictionary.h"
#include "object_visitor.h"
#include "tick_interface.h"
#include <client_context/client_context.h>
#include <algorithm>
#include <cassert>
#include <string_view>

namespace decof {

object_dictionary::context_guard::context_guard(object_dictionary& od, client_context* cc) : object_dictionary_(od)
{
    object_dictionary_.set_current_context(cc);
}

object_dictionary::context_guard::~context_guard()
{
    object_dictionary_.set_current_context(nullptr);
}

object_dictionary::object_dictionary(const std::string& root_uri) : node(root_uri, nullptr)
{
}

void object_dictionary::add_context(std::shared_ptr<client_context> client_context)
{
    client_contexts_.push_back(client_context);
}

void object_dictionary::remove_context(std::shared_ptr<client_context> client_context)
{
    client_contexts_.remove(client_context);
}

const std::shared_ptr<client_context> object_dictionary::current_context() const
{
    // FIXME
    //    assert(current_context_ != nullptr);
    return current_context_;
}

void object_dictionary::register_for_tick(tick_interface* tick_target)
{
    tick_targets_.push_back(tick_target);
}

void object_dictionary::unregister_for_tick(tick_interface* tick_target)
{
    tick_targets_.remove(tick_target);
}

object* object_dictionary::find_object(std::string_view uri, char separator)
{
    if (uri.empty()) {
        return nullptr;
    }

    if (uri[0] == separator) {
        uri.remove_prefix(1);
    }

    auto sep_idx = uri.find(separator);

    if (name() != uri.substr(0, sep_idx)) {
        return nullptr;
    }

    if (sep_idx == std::string_view::npos) {
        return this;
    }

    uri.remove_prefix(sep_idx + 1);

    return find_descendant_object(uri, separator);
}

void object_dictionary::set_current_context(client_context* client_context)
{
    assert(
        (current_context_ == nullptr && client_context != nullptr) ||
        (current_context_ != nullptr && client_context == nullptr));
    if (client_context != nullptr)
        current_context_ = client_context->shared_from_this();
    else
        current_context_.reset();
}

void object_dictionary::tick()
{
    for (auto& elem : tick_targets_) {
        elem->tick();
    }
}

} // namespace decof
