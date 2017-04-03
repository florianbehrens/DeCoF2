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

#include "exceptions.h"

namespace decof
{

parse_error::parse_error()
  : decof_error("Parse error")
{}

bad_request_error::bad_request_error()
  : decof_error("Bad request")
{}

access_denied_error::access_denied_error()
  : decof_error("Access denied")
{}

invalid_parameter_error::invalid_parameter_error()
  : decof_error("Invalid parameter")
{}

wrong_type_error::wrong_type_error()
  : decof_error("Wrong type")
{}

invalid_value_error::invalid_value_error()
  : decof_error("Invalid value")
{}

unknown_operation_error::unknown_operation_error()
  : decof_error("Unknown operation")
{}

invalid_userlevel_error::invalid_userlevel_error()
    : decof_error("Invalid userlevel")
{}

not_subscribed_error::not_subscribed_error()
    : decof_error("Not subscribed")
{}

not_implemented_error::not_implemented_error()
    : decof_error("Not implemented")
{}

} // namespace decof
