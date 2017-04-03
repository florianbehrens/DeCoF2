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

namespace decof
{

namespace websocket
{

const char* websocket_error_category_impl::name() const noexcept
{
    return "Websocket error category";
}

std::string websocket_error_category_impl::message(int condition) const
{
    static const std::map<error, const char*> error_codes_map = {
        { error::parse_error,         "Parse error" },
        { error::invalid_request,     "Invalid request" },
        { error::method_not_found,    "Method not found" },
        { error::invalid_params,      "Invalid params" },
        { error::internal_error,      "Internal error" },
        { error::access_denied,       "Access denied" },
        { error::invalid_parameter,   "Invalid parameter" },
        { error::wrong_type,          "Wrong type" },
        { error::invalid_value,       "Invalid value" },
        { error::not_subscribed,      "Not subscribed" },
        { error::not_implemented,     "Not implemented" }
    };

    const auto it = error_codes_map.find(static_cast<error>(condition));
    if (it == error_codes_map.cend())
        return "Unknown error";

    return it->second;
}

std::error_code make_error_code(error e)
{
    static websocket_error_category_impl ecat;
    return std::error_code(static_cast<int>(e), ecat);
}

error error_code_from_exception(const decof_error& ex)
{
    using std::type_index;

    static const std::map<std::type_index, error> error_codes_map = {
        { type_index(typeid(parse_error)),              error::parse_error },
        { type_index(typeid(access_denied_error)),      error::access_denied },
        { type_index(typeid(invalid_parameter_error)),  error::invalid_parameter },
        { type_index(typeid(wrong_type_error)),         error::wrong_type },
        { type_index(typeid(invalid_value_error)),      error::invalid_value },
        { type_index(typeid(unknown_operation_error)),  error::method_not_found },
        { type_index(typeid(invalid_userlevel_error)),  error::invalid_userlevel },
        { type_index(typeid(not_subscribed_error)),     error::not_subscribed },
        { type_index(typeid(not_implemented_error)),    error::not_implemented }
    };

    auto it = error_codes_map.find(type_index(typeid(ex)));
    if (it != error_codes_map.cend())
        return it->second;

    return error::internal_error;
}

} // namespace websocket

} // namespace decof
