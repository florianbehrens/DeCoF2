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

#include "client_context.h"

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
{
    io_service_ptr_ = std::make_shared<boost::asio::io_service>();
    fast_timer_ptr_.reset(new regular_timer(*io_service_ptr_.get(), std::chrono::milliseconds(50)));
    medium_timer_ptr_.reset(new regular_timer(*io_service_ptr_.get(), std::chrono::milliseconds(500)));
    slow_timer_ptr_.reset(new regular_timer(*io_service_ptr_.get(), std::chrono::milliseconds(5000)));

    fast_timer_ptr_->start();
    medium_timer_ptr_->start();
    slow_timer_ptr_->start();
}

regular_timer& object_dictionary::get_timer()
{
    return *medium_timer_ptr_.get();
}

std::shared_ptr<boost::asio::io_service> object_dictionary::io_service()
{
    return io_service_ptr_;
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

} // namespace decof
