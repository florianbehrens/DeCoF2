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

#include "endian.h"
#include "typed_array_value_encoder.h"

namespace decof
{

namespace scgi
{

void typed_array_value_encoder::encode_boolean_seq(std::ostream &out, const boolean_seq &value)
{
    for (const auto &elem : value)
        out.put(static_cast<unsigned char>(elem));
}

void typed_array_value_encoder::encode_integer_seq(std::ostream &out, const integer_seq &value)
{
    for (const auto &elem : value) {
        int32_t elem_le = native_to_little_endian(elem);
        out.write(reinterpret_cast<const char*>(&elem_le), sizeof(elem_le));
    }
}

void typed_array_value_encoder::encode_real_seq(std::ostream &out, const real_seq &value)
{
    for (const auto &elem : value) {
        double elem_le = native_to_little_endian(elem);
        out.write(reinterpret_cast<const char*>(&elem_le), sizeof(elem_le));
    }
}

void typed_array_value_encoder::encode_string_seq(std::ostream &out, const string_seq &value)
{
    // Bencode (see http://en.wikipedia.org/wiki/Bencode) encoder
    for (const auto &elem : value)
        out << elem.size() << ":" << elem << "\r\n";
}

void typed_array_value_encoder::encode_binary_seq(std::ostream &out, const binary_seq &value)
{
    // Bencode (see http://en.wikipedia.org/wiki/Bencode) encoder
    for (const auto &elem : value)
        out << elem.size() << ":" << elem << "\r\n";
}

void typed_array_value_encoder::encode_tuple(std::ostream &out, const dynamic_tuple &value)
{
    for (const auto &elem : value) {
        if (elem.type() == typeid(decof::string)) {
            decof::string value = boost::any_cast<decof::string>(elem);
            out << value.size() << ":" << value;
        } else if (elem.type() == typeid(decof::binary)) {
            decof::binary value = boost::any_cast<decof::binary>(elem);
            out << value.size() << ":" << value;
        } else
            encode_any(out, elem);
        out << "\r\n";
    }
}

} // namespace scgi

} // namespace decof
