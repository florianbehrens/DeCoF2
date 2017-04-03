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

#ifndef DECOF_WEBSOCKET_ERROR_H
#define DECOF_WEBSOCKET_ERROR_H

#include <exception>
#include <string>
#include <system_error>

#include <decof/exceptions.h>

namespace decof {
namespace websocket {

/**
 * @brief JSON-RPC error codes.
 *
 * See http://www.jsonrpc.org/specification for a list of errors.
 */
enum class error {
    parse_error             = -32700,
    invalid_request         = -32600,
    method_not_found        = -32601,
    invalid_params          = -32602,
    internal_error          = -32603,

    // Implementation defined errors
    access_denied           = -32000,
    invalid_parameter       = -32001,
    wrong_type              = -32002,
    invalid_value           = -32003,
    invalid_userlevel       = -32004,
    not_subscribed          = -32005, // TODO?
    not_implemented         = -32006  // TODO?
};

struct invalid_params_error
{};

struct websocket_error_category_impl : public std::error_category
{
    virtual const char* name() const noexcept override;
    virtual std::string message(int condition) const override;
};

std::error_code make_error_code(error e);

error error_code_from_exception(const decof_error& ex);

} // namespace websocket

} // namespace decof

namespace std
{

template<>
struct is_error_code_enum<decof::websocket::error>
{
    static const bool value = true;
};

} // namespace std

#endif // DECOF_WEBSOCKET_ERROR_H
