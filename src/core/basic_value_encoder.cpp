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

#include "basic_value_encoder.h"

#include <iomanip>

#include "exceptions.h"

namespace decof
{

basic_value_encoder::~basic_value_encoder()
{}

void basic_value_encoder::encode_any(std::string &str, const boost::any &any_value)
{
    std::stringstream out(str);
    encode_any(out, any_value);
}

void basic_value_encoder::encode_any(std::stringstream &out, const boost::any &any_value)
{
    if (any_value.type() == typeid(boolean))
        encode(out, boost::any_cast<boolean>(any_value));
    else if (any_value.type() == typeid(integer))
        encode(out, boost::any_cast<integer>(any_value));
    else if (any_value.type() == typeid(real))
        encode(out, boost::any_cast<real>(any_value));
    else if (any_value.type() == typeid(string))
        encode(out, boost::any_cast<string>(any_value));
    else if (any_value.type() == typeid(binary))
        encode(out, boost::any_cast<binary>(any_value));
    else if (any_value.type() == typeid(boolean_seq))
        encode(out, boost::any_cast<boolean_seq>(any_value));
    else if (any_value.type() == typeid(integer_seq))
        encode(out, boost::any_cast<integer_seq>(any_value));
    else if (any_value.type() == typeid(real_seq))
        encode(out, boost::any_cast<real_seq>(any_value));
    else if (any_value.type() == typeid(string_seq))
        encode(out, boost::any_cast<string_seq>(any_value));
    else if (any_value.type() == typeid(binary_seq))
        encode(out, boost::any_cast<binary_seq>(any_value));
    else if (any_value.type() == typeid(dynamic_tuple))
        encode(out, boost::any_cast<dynamic_tuple>(any_value));
    else
        throw wrong_type_error();
}

void basic_value_encoder::encode(std::stringstream &out, const boolean &value)
{
    out << value;
}

void basic_value_encoder::encode(std::stringstream &out, const integer &value)
{
    out << value;
}

void basic_value_encoder::encode(std::stringstream &out, const real &value)
{
    out << value;
}

void basic_value_encoder::encode(std::stringstream &out, const string &value)
{
    out << value;
}

void basic_value_encoder::encode(std::stringstream &out, const binary &value)
{
    out << value;
}

void basic_value_encoder::encode(std::stringstream &out, const boolean_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode(std::stringstream &out, const integer_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode(std::stringstream &out, const real_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode(std::stringstream &out, const string_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode(std::stringstream &out, const binary_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode(std::stringstream &out, const dynamic_tuple &value)
{
    out << "{";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode_any(out, *it);
    }
    out << "}";
}

} // namespace decof
