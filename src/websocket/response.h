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

#ifndef DECOF_WEBSOCKET_RESPONSE_H
#define DECOF_WEBSOCKET_RESPONSE_H

#include <exception>

#include <boost/any.hpp>

#include "request.h"

namespace decof
{

namespace websocket
{

/**
 * @brief JSON-RPC response class.
 *
 * Represents a JSON-RPC 2.0 response.
 */
class response
{
public:
    static response from_request(const request& request);

    bool has_id() const;

    id_type id;
    boost::any result;
    std::error_code error_code;
};

/**
 * @brief Insertion operator for JSON-RPC response.
 *
 * The following is an example of a (pretty printed) serialized JSON response
 * object:
 *
 * @code
 * {
 *     result: true,        // Value in case of a successful get method
 *     error: {             // Only in error case
 *         code: -32601,
 *         message: "Method not found"
 *     },
 *     id: 123
 * }
 * @endcode
 */
std::ostream& operator<<(std::ostream& out, const response& r);

} // namespace websocket

} // namespace decof

#endif // DECOF_WEBSOCKET_RESPONSE_H
