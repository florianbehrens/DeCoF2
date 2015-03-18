/*
 * Copyright (c) 2015 Florian Behrens
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

#include "js_value_encoder.h"

namespace
{

template<typename T>
T native_to_little_endian(const T &value)
{
    T retval = value;

    // System is big endian?
    uint32_t test = 1;
    if (*reinterpret_cast<const char*>(&test) == '\001') {
        const size_t size = sizeof(T);
        const char *input = reinterpret_cast<const char *>(&value);
        char *output = reinterpret_cast<char *>(&retval);
        for (size_t i = 0; i < size; ++i)
            output[i] = input[size-1-i];
    }

    return retval;
}

} // Anonymous namespace

namespace decof
{

namespace scgi
{

void js_value_encoder::encode(std::ostream &out, const boolean &value)
{
    if (value == false)
        out << "false";
    else
        out << "true";
}

void js_value_encoder::encode(std::ostream &out, const boolean_seq &value)
{
    for (const auto &elem : value)
        out.put(static_cast<unsigned char>(elem));
}

void js_value_encoder::encode(std::ostream &out, const integer_seq &value)
{
    for (const auto &elem : value) {
        uint32_t elem_le = native_to_little_endian(elem);
        out.write(reinterpret_cast<const char*>(&elem_le), sizeof(elem_le));
    }
}

void js_value_encoder::encode(std::ostream &out, const real_seq &value)
{
    for (const auto &elem : value) {
        double elem_le = native_to_little_endian(elem);
        out.write(reinterpret_cast<const char*>(&elem_le), sizeof(elem_le));
    }
}

void js_value_encoder::encode(std::ostream &out, const string_seq &value)
{
    for (const auto &elem : value)
        out << html_string_escape(elem) << "\r\n";
}

void js_value_encoder::encode(std::ostream &out, const binary_seq &value)
{
    for (const auto &elem : value)
        out << base64_encode(elem) << "\r\n";
}

void js_value_encoder::encode(std::ostream &out, const dynamic_tuple &value)
{
    for (const auto &elem : value) {
        encode_any(out, elem);
        out << "\r\n";
    }
}

} // namespace scgi

} // namespace decof
