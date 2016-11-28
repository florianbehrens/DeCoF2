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

#include "encoder.h"

#include <map>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <decof/conversion.h>
#include <decof/exceptions.h>
#include <decof/types.h>

namespace {

/// Base64 string encoder function.
std::string base64_encode(const decof::binary &bin)
{
    // The following is based on code from
    // http://stackoverflow.com/questions/10521581/base64-encode-using-boost-throw-exception/10973348#10973348
    using namespace boost::archive::iterators;

    typedef base64_from_binary<transform_width<std::string::const_iterator, 6, 8>> it_base64_t;

    unsigned int writePaddChars = (3 - bin.length() % 3) % 3;
    std::string base64(it_base64_t(bin.begin()), it_base64_t(bin.end()));
    base64.append(writePaddChars, '=');

    return base64;
}

} // Anonymous namespace

namespace decof
{

namespace cli
{

void encoder::encode_boolean(std::ostream &out, const boolean &value)
{
    if (value == true)
        out << "#t";
    else
        out << "#f";
}

void encoder::encode_string(std::ostream &out, const string &value)
{
    static const std::map<char, char> escape_characters = {
        { '\a', 'a' },
        { '\b', 'b' },
        { '\f', 'f' },
        { '\n', 'n' },
        { '\r', 'r' },
        { '\t', 't' },
        { '\v', 'v' },
        { '\\', '\\' },
        { '\'', '\'' },
        { '\"', '"' },
        { '\?', '?' }
    };

    out << "\"";

    for (const auto& ch : value) {
        auto it = escape_characters.cbegin();

        if (ch >= 0x20 && ch <= 0x7F && ch != '\\' && ch != '\'' && ch != '\"' && ch != '\?') {
            out << ch;
        } else if ((it = escape_characters.find(ch)) != escape_characters.cend()) {
            out << "\\" << it->second;
        } else {
            out << "\\x" << std::hex << ch;
        }
    }

    out << "\"";
}

void encoder::encode_binary(std::ostream &out, const binary &value)
{
    out << "&" << base64_encode(value);
}

} // namespace cli

} // namespace decof
