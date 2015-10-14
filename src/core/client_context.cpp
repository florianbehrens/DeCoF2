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

#include "client_context.h"

#include "client_write_interface.h"
#include "connection.h"
#include "event.h"
#include "exceptions.h"
#include "object.h"
#include "object_dictionary.h"

namespace decof
{

client_context::client_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection, userlevel_t userlevel)
  : object_dictionary_(a_object_dictionary),
    connection_(connection),
    userlevel_(userlevel)
{}

client_context::~client_context()
{}

userlevel_t client_context::userlevel() const
{
    return userlevel_;
}

void client_context::userlevel(userlevel_t ul)
{
    userlevel_ = ul;
}

std::string client_context::connection_type() const
{
    return std::string("undefined");
}

std::string client_context::remote_endpoint() const
{
    return std::string("undefined");
}

void client_context::preload()
{}

void client_context::set_parameter(const std::string &uri, const boost::any &any_value, char separator)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    object *te = object_dictionary_.find_object(uri, separator);
    client_write_interface* param = dynamic_cast<client_write_interface*>(te);

    if (param == nullptr)
        throw invalid_parameter_error();
    if (userlevel_ < te->writelevel())
        throw access_denied_error();

    param->value(any_value);
}

boost::any client_context::get_parameter(const std::string &uri, char separator)
{
    boost::any retval;
    object_dictionary::context_guard cg(object_dictionary_, this);

    object *obj = object_dictionary_.find_object(uri, separator);
    client_read_interface* param = dynamic_cast<client_read_interface*>(obj);

    if (param == nullptr)
        throw invalid_parameter_error();
    if (userlevel_ < obj->readlevel())
        throw access_denied_error();

    return param->any_value();
}

void client_context::signal_event(const std::string &uri, char separator)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    object *te = object_dictionary_.find_object(uri, separator);
    event* ev = dynamic_cast<event*>(te);

    if (ev == nullptr)
        throw invalid_parameter_error();
    if (userlevel_ < te->writelevel())
        throw access_denied_error();

    ev->signal();
}

void client_context::observe(const std::string &uri, client_read_interface::signal_type::slot_type slot, char separator)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    auto obj = object_dictionary_.find_object(uri, separator);

    if (userlevel_ < obj->readlevel())
        throw access_denied_error();
    if (client_read_interface* param = dynamic_cast<client_read_interface*>(obj)) {
        if (observables_.count(uri) == 0) {
            boost::signals2::connection connection = param->observe(slot);
            observables_.emplace(uri, connection);
        }
    } else
        throw invalid_parameter_error();
}

void client_context::unobserve(const std::string &uri)
{
    try {
        observables_.at(uri).disconnect();
        observables_.erase(uri);
    } catch (std::out_of_range&) {
    }
}

void client_context::browse(object_visitor *visitor, const std::string &root_uri)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    std::string uri = root_uri;
    if (uri.empty())
        uri = object_dictionary_.name();

    if (object *te = object_dictionary_.find_object(uri)) {
        // Recursively iterate over all objects beginning from root URI
        browse_object(te, visitor);
    } else
        throw invalid_parameter_error();
}

void client_context::tick()
{
    object_dictionary_.tick();
}

void client_context::browse_object(object *te, object_visitor *visitor)
{
    te->accept(visitor);

    // If node browse children
    if (node *n = dynamic_cast<node*>(te)) {
        for (auto &child : *n)
            browse_object(child, visitor);
    }
}

} // namespace decof
