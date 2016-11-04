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

runtime_error::runtime_error(int code, const std::string& what)
  : std::runtime_error(what),
    code_(code)
{}

int runtime_error::code()
{
    return code_;
}

access_denied_error::access_denied_error()
  : runtime_error(ACCESS_DENIED_ERROR, "Access denied error")
{}

invalid_parameter_error::invalid_parameter_error()
  : runtime_error(INVALID_PARAMETER_ERROR, "Invalid parameter error")
{}

wrong_type_error::wrong_type_error()
  : runtime_error(WRONG_TYPE_ERROR, "Wrong type error")
{}

invalid_value_error::invalid_value_error()
  : runtime_error(INVALID_VALUE_ERROR, "Invalid value error")
{}

unknown_operation_error::unknown_operation_error()
  : runtime_error(UNKNOWN_OPERATION_ERROR, "Unknown operation error")
{}

parse_error::parse_error()
  : runtime_error(PARSE_ERROR, "Parse error")
{}

not_implemented_error::not_implemented_error()
    : runtime_error(NOT_IMPLEMENTED_ERROR, "Not implemented")
{}

invalid_userlevel_error::invalid_userlevel_error()
    : runtime_error(INVALID_USERLEVEL, "Invalid userlevel")
{}

} // namespace decof
