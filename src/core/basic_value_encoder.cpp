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

std::string basic_value_encoder::html_string_escape(const std::string &str)
{
    return boost::find_format_all_copy(str, boost::token_finder(!boost::is_print() || boost::is_any_of("\"")), string_escaper());
}

std::string basic_value_encoder::base64_encode(const binary &bin)
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

void basic_value_encoder::encode_any(std::string &str, const boost::any &any_value)
{
    std::stringstream out;
    encode_any(out, any_value);
    str = std::move(out.str());
}

void basic_value_encoder::encode_any(std::ostream &out, const boost::any &any_value)
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

void basic_value_encoder::encode(std::ostream &out, const boolean &value)
{
    out << value;
}

void basic_value_encoder::encode(std::ostream &out, const integer &value)
{
    out << value;
}

void basic_value_encoder::encode(std::ostream &out, const real &value)
{
    out << value;
}

void basic_value_encoder::encode(std::ostream &out, const string &value)
{
    out << value;
}

void basic_value_encoder::encode(std::ostream &out, const binary &value)
{
    out << value;
}

void basic_value_encoder::encode(std::ostream &out, const boolean_seq &value)
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

void basic_value_encoder::encode(std::ostream &out, const integer_seq &value)
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

void basic_value_encoder::encode(std::ostream &out, const real_seq &value)
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

void basic_value_encoder::encode(std::ostream &out, const string_seq &value)
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

void basic_value_encoder::encode(std::ostream &out, const binary_seq &value)
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

void basic_value_encoder::encode(std::ostream &out, const dynamic_tuple &value)
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
