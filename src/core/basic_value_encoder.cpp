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

#if 0

#include <client_context/basic_value_encoder.h>

#include <iomanip>
#include <sstream>

#include <boost/format.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/find_format.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "exceptions.h"

namespace {

struct string_escaper
{
    template<typename T>
    std::string operator()(const T& match) const
    {
        std::string s;
        typename T::const_iterator i = match.begin();
        for (; i != match.end(); i++) {
            unsigned char c = *i;
            s += boost::str(boost::format("%%%02X") % static_cast<unsigned int>(c));
        }
        return s;
    }
};

} // Anonymous namespace

namespace decof
{

basic_value_encoder::~basic_value_encoder()
{}

void basic_value_encoder::encode_any(std::string &str, const boost::any &any_value)
{
    std::stringstream out;
    encode_any(out, any_value);
    str = std::move(out.str());
}

void basic_value_encoder::encode_any(std::ostream &out, const boost::any &any_value)
{
    if (any_value.type() == typeid(boolean))
        encode_boolean(out, boost::any_cast<boolean>(any_value));
    else if (any_value.type() == typeid(integer))
        encode_integer(out, boost::any_cast<integer>(any_value));
    else if (any_value.type() == typeid(real))
        encode_real(out, boost::any_cast<real>(any_value));
    else if (any_value.type() == typeid(string))
        encode_string(out, boost::any_cast<string>(any_value));
    else if (any_value.type() == typeid(binary))
        encode_binary(out, boost::any_cast<binary>(any_value));
    else if (any_value.type() == typeid(boolean_seq))
        encode_boolean_seq(out, boost::any_cast<boolean_seq>(any_value));
    else if (any_value.type() == typeid(integer_seq))
        encode_integer_seq(out, boost::any_cast<integer_seq>(any_value));
    else if (any_value.type() == typeid(real_seq))
        encode_real_seq(out, boost::any_cast<real_seq>(any_value));
    else if (any_value.type() == typeid(string_seq))
        encode_string_seq(out, boost::any_cast<string_seq>(any_value));
    else if (any_value.type() == typeid(binary_seq))
        encode_binary_seq(out, boost::any_cast<binary_seq>(any_value));
    else if (any_value.type() == typeid(dynamic_tuple))
        encode_tuple(out, boost::any_cast<dynamic_tuple>(any_value));
    else
        throw wrong_type_error();
}

void basic_value_encoder::encode_boolean(std::ostream &out, const boolean &value)
{
    out << value;
}

void basic_value_encoder::encode_integer(std::ostream &out, const integer &value)
{
    out << value;
}

void basic_value_encoder::encode_real(std::ostream &out, const real &value)
{
    // Use minimum value without loss of precision in conversion between binary
    // and decimal number representation and vice versa.
    out << std::setprecision(17) << value;
}

void basic_value_encoder::encode_string(std::ostream &out, const string &value)
{
    out << value;
}

void basic_value_encoder::encode_binary(std::ostream &out, const binary &value)
{
    out << value;
}

void basic_value_encoder::encode_boolean_seq(std::ostream &out, const boolean_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode_boolean(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode_integer_seq(std::ostream &out, const integer_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode_integer(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode_real_seq(std::ostream &out, const real_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode_real(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode_string_seq(std::ostream &out, const string_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode_string(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode_binary_seq(std::ostream &out, const binary_seq &value)
{
    out << "[";
    auto it = value.cbegin();
    for (; it != value.cend(); ++it) {
        if (it != value.cbegin())
            out << ",";
        encode_binary(out, *it);
    }
    out << "]";
}

void basic_value_encoder::encode_tuple(std::ostream &out, const dynamic_tuple &value)
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
#endif
