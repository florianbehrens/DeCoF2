/*
 * Copyright (c) 2019 Florian Behrens
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

#include <decof/client_context/basic_client_context.h>
#include <decof/client_write_interface.h>
#include <decof/event.h>
#include <decof/exceptions.h>
#include <decof/object_dictionary.h>
#include <decof/userlevel.h>

namespace decof {

basic_client_context::basic_client_context(object_dictionary& od, userlevel_t ul)
  : object_dictionary_(od), userlevel_(ul)
{
}

userlevel_t basic_client_context::userlevel() const
{
    return userlevel_;
}

userlevel_t basic_client_context::effective_userlevel() const
{
    return (userlevel_ == decof::Readonly ? decof::Normal : userlevel_);
}

void basic_client_context::userlevel(userlevel_t ul)
{
    if (ul < Minimum || ul > Maximum)
        throw invalid_userlevel_error();

    userlevel_ = ul;
}

std::string basic_client_context::connection_type() const
{
    return std::string("undefined");
}

std::string basic_client_context::remote_endpoint() const
{
    return std::string("undefined");
}

void basic_client_context::set_parameter(const std::string& uri, const value_t& value, char separator)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    if (userlevel_ == decof::Readonly)
        throw access_denied_error();

    object*                 te    = object_dictionary_.find_object(uri, separator);
    client_write_interface* param = dynamic_cast<client_write_interface*>(te);

    if (param == nullptr)
        throw invalid_parameter_error();
    if (userlevel_ > te->writelevel())
        throw access_denied_error();

    param->generic_value(value);
}

value_t basic_client_context::get_parameter(const std::string& uri, char separator)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    object*                obj   = object_dictionary_.find_object(uri, separator);
    client_read_interface* param = dynamic_cast<client_read_interface*>(obj);

    if (param == nullptr)
        throw invalid_parameter_error();
    if (effective_userlevel() > obj->readlevel())
        throw access_denied_error();

    return param->generic_value();
}

void basic_client_context::signal_event(const std::string& uri, char separator)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    if (userlevel_ == decof::Readonly)
        throw access_denied_error();

    object* te = object_dictionary_.find_object(uri, separator);
    event*  ev = dynamic_cast<event*>(te);

    if (ev == nullptr)
        throw invalid_parameter_error();
    if (userlevel_ > te->writelevel())
        throw access_denied_error();

    ev->signal();
}

void basic_client_context::browse(object_visitor* visitor, const std::string& root_uri)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    std::string uri = root_uri;
    if (uri.empty())
        uri = object_dictionary_.name();

    if (object* te = object_dictionary_.find_object(uri)) {
        // Recursively iterate over all objects beginning from root URI
        browse_object(te, visitor);
    } else
        throw invalid_parameter_error();
}

void basic_client_context::browse_object(object* te, object_visitor* visitor)
{
    te->accept(visitor);

    // If node browse children
    if (node* n = dynamic_cast<node*>(te)) {
        for (auto& child : *n) {
            if (effective_userlevel() <= child->readlevel())
                browse_object(child, visitor);
        }
    }
}

} // namespace decof
