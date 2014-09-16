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

#include "basic_readwrite_parameter.h"
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

const connection *client_context::connnection() const
{
    return connection_.get();
}

void client_context::set_parameter(const std::string &uri, const boost::any &any_value)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    object *te = object_dictionary_.find_object(uri);
    if (te != nullptr && userlevel_ >= te->writelevel()) {
        if (basic_readwrite_parameter* parameter = dynamic_cast<basic_readwrite_parameter*>(te))
            parameter->set_private_value(any_value);
        else
            throw access_denied_error();
    } else
        throw invalid_parameter_error();
}

boost::any client_context::get_parameter(const std::string &uri)
{
    boost::any retval;
    object_dictionary::context_guard cg(object_dictionary_, this);

    basic_parameter* param = dynamic_cast<basic_parameter*>(object_dictionary_.find_object(uri));
    if (param != nullptr && userlevel_ >= param->readlevel())
        retval = param->any_value();
    else
        throw invalid_parameter_error();

    return retval;
}

void client_context::signal_event(const std::string &uri)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    object *te = object_dictionary_.find_object(uri);
    if (te != nullptr && userlevel_ >= te->writelevel()) {
        if (event* ev = dynamic_cast<event*>(te))
            ev->signal();
        else
            throw wrong_type_error();
    } else
        throw invalid_parameter_error();
}

void client_context::observe(const std::string &uri, object::signal_type::slot_type slot)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    if (basic_parameter* param = dynamic_cast<basic_parameter*>(object_dictionary_.find_object(uri))) {
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

void client_context::browse(const std::string &root_uri, object_visitor *visitor)
{
    object_dictionary::context_guard cg(object_dictionary_, this);

    if (object *te = object_dictionary_.find_object(root_uri)) {
        // Recursively iterate over all objects beginning from root URI
        browse_object(te, visitor);
    } else
        throw invalid_parameter_error();
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
