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
#include <boost/algorithm/string.hpp>
#include <client_context/client_context.h>
#include <client_context/object_visitor.h>

namespace decof
{

object_dictionary::context_guard::context_guard(object_dictionary& od, client_context* cc) :
    object_dictionary_(od)
{
    object_dictionary_.set_current_context(cc);
}

object_dictionary::context_guard::~context_guard()
{
    object_dictionary_.set_current_context(nullptr);
}

object_dictionary::object_dictionary(const std::string &root_uri)
 : node(root_uri, nullptr)
{}

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

object_dictionary::tick_connection object_dictionary::register_for_tick(object_dictionary::tick_slot_type slot)
{
    return tick_signal_.connect(slot);
}

object *object_dictionary::find_object(const std::string &curi, char separator)
{
    std::string uri(curi);

    // Ignore leading separator character
    if (uri.length() > 0 && uri[0] == separator)
        uri = curi.substr(1);

    if (uri == name())
        return this;

    if (boost::algorithm::starts_with(uri, name() + separator)) {
        std::string sub_uri = uri.substr(uri.find(separator) + 1);
        return find_child(sub_uri, separator);
    }

    return nullptr;
}

void object_dictionary::set_current_context(client_context *client_context)
{
    assert((current_context_ == nullptr && client_context != nullptr) ||
           (current_context_ != nullptr && client_context == nullptr));
    if (client_context != nullptr)
        current_context_ = client_context->shared_from_this();
    else
        current_context_.reset();
}

void object_dictionary::tick()
{
    tick_signal_();
}

} // namespace decof
