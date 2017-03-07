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

#include "response.h"

#include "json_value_encoder.h"

namespace decof
{

namespace websocket
{

response response::from_request(const request& request)
{
    return response{ request.id };
}

bool response::has_id() const
{
    return id.which() != 0;
}

std::ostream& operator<<(std::ostream& out, const response& r)
{
    json_value_encoder enc;

    out << "{\"jsonrpc\":\"2.0\",";

    if (! r.error_code) {
        out << "\"result\":";

        if (! r.result.empty())
            enc.encode_any(out, r.result);
        else
            out << "null";

        out  << ',';
    } else {
        out << "\"error\":{"
            << "\"code\":";

        enc.encode_integer(out, r.error_code.value());

        out << ","
            << "\"message\":";

        enc.encode_string(out, r.error_code.message());

        out << "},";
    }

    out << "\"id\":";

    if (r.has_id())
        out << r.id;
    else
        out << "null";

    out << "}";

    return out;
}

} // namespace websocket

} // namespace decof
