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

#include "json_value_encoder.h"

#include <map>

namespace decof
{

void json_value_encoder::encode_string(std::ostream& out, const string& value)
{
    // See http://www.json.org for supported escape characters in JSON.
    static const std::map<char, char> escape_characters = {
        { '"',  '"' },
        { '\\', '\\' },
        { '/',  '/' },
        { '\b', 'b' },
        { '\f', 'f' },
        { '\n', 'n' },
        { '\r', 'r' },
        { '\t', 't' }
    };

    out << '"';

    for (const auto& ch : value) {
        auto it = escape_characters.cbegin();

        if (ch >= 0x20 && ch != '\\' && ch != '\"') {
            out << ch;
        } else if ((it = escape_characters.find(ch)) != escape_characters.cend()) {
            out << "\\" << it->second;
        } else {
            out << "\\x" << std::hex << ch;
        }
    }

    out << '"';
}

void json_value_encoder::encode_binary(std::ostream &out, const binary &value)
{
    // Not supported
    out << "null";
}

void json_value_encoder::encode_tuple(std::ostream &out, const dynamic_tuple &value)
{
    out << '[';

    for (auto it = value.cbegin(); it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ',';

        encode_any(out, *it);
    }

    out << ']';
}

} // namespace decof
