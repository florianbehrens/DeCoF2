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

#include "errors.h"

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
    switch (static_cast<error_codes>(condition)) {
    case error_codes::parse_error:
        return "Parse error";
        break;
    case error_codes::invalid_request:
        return "Invalid request";
        break;
    case error_codes::method_not_found:
        return "Method not found";
        break;
    case error_codes::invalid_params:
        return "Invalid params";
        break;
    case error_codes::internal_error:
    default:
        return "Internal error";
        break;
    }
}

std::error_code make_error_code(error_codes e)
{
    static websocket_error_category_impl ecat;
    return std::error_code(static_cast<int>(e), ecat);
}

} // namespace websocket

} // namespace decof
