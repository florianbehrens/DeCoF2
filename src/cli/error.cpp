/*
 * Copyright (c) 2017 Florian Behrens
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

#include "error.h"

#include <map>
#include <typeindex>
#include <typeinfo>

namespace decof {
namespace cli {

size_t error_code_from_exception(const decof_error& ex)
{
    using std::type_index;

    static const std::map<std::type_index, error> error_codes_map = {
        { type_index(typeid(parse_error)),              PARSE_ERROR },
        { type_index(typeid(access_denied_error)),      ACCESS_DENIED },
        { type_index(typeid(invalid_parameter_error)),  INVALID_PARAMETER },
        { type_index(typeid(wrong_type_error)),         WRONG_TYPE },
        { type_index(typeid(invalid_value_error)),      INVALID_VALUE },
        { type_index(typeid(unknown_operation_error)),  UNKNOWN_OPERATION },
        { type_index(typeid(invalid_userlevel_error)),  INVALID_USERLEVEL },
        { type_index(typeid(not_subscribed_error)),     NOT_SUBSCRIBED },
        { type_index(typeid(not_implemented_error)),    NOT_IMPLEMENTED }
    };

    size_t errcode = UNKNOWN_ERROR;

    try {
        errcode = error_codes_map.at(type_index(typeid(ex)));
    } catch (...) {}

    return errcode;
}

} } // namespace cli, decof
