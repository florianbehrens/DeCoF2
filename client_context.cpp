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
#include "exceptions.h"
#include "object_dictionary.h"
#include "tree_element.h"

namespace decof
{

client_context::client_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection)
  : object_dictionary_(a_object_dictionary),
    connection_(connection)
{}

client_context::~client_context()
{}

const connection *client_context::connnection() const
{
    return connection_.get();
}

void client_context::set_parameter(const std::string &uri, const boost::any &any_value)
{
    object_dictionary::context_guard(object_dictionary_, this);

    if (tree_element *te = object_dictionary_.find_object(uri)) {
        if (basic_readwrite_parameter* parameter = dynamic_cast<basic_readwrite_parameter*>(te))
            parameter->set_private_value(any_value);
        else
            throw access_denied_error();
    } else
        throw invalid_parameter_error();
}

boost::any client_context::get_parameter(const std::string &uri)
{
    object_dictionary::context_guard(object_dictionary_, this);

    tree_element *te = object_dictionary_.find_object(uri);
    if (te == nullptr)
        throw invalid_parameter_error();

    return te->any_value();
}

void client_context::observe(const std::string &uri, tree_element::signal_type::slot_type slot)
{
    if (tree_element *te = object_dictionary_.find_object(uri)) {
        if (observables_.count(uri) == 0) {
            tree_element::connection connection = te->observe(slot);
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

} // namespace decof
