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

#include "connection.h"

namespace decof
{

client_context::client_context(object_dictionary& a_object_dictionary, connection *a_connection)
  : object_dictionary_(a_object_dictionary),
    connection_(a_connection)
{}

client_context::~client_context()
{}

const connection *client_context::connnection() const
{
    return connection_.get();
}

void client_context::set_parameter(const std::string &uri, const boost::any &value)
{}

boost::any client_context::get_parameter(const std::string &uri)
{
    assert(false);
    return boost::any();
}

} // namespace decof
