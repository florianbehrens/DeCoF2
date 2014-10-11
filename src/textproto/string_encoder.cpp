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

#include "string_encoder.h"

#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/lexical_cast.hpp>

#include "conversion.h"
#include "exceptions.h"
#include "types.h"

namespace {

// The following two functions are based on code from
// http://stackoverflow.com/questions/10521581/base64-encode-using-boost-throw-exception/10973348#10973348
inline std::string base64encode(const decof::binary &bin)
{
    using namespace boost::archive::iterators;

    typedef base64_from_binary<transform_width<std::string::const_iterator, 6, 8>> it_base64_t;

    unsigned int writePaddChars = (3 - bin.length() % 3) % 3;
    std::string base64(it_base64_t(bin.begin()), it_base64_t(bin.end()));
    base64.append(writePaddChars, '=');

    return base64;
}

inline std::string encode_boolean(decof::boolean value)
{
    if (value == true)
        return std::string("#t");
    return std::string("#f");
}

inline std::string encode_integer(decof::integer value)
{
    return boost::lexical_cast<std::string>(value);
}

inline std::string encode_real(decof::real value)
{
    return boost::lexical_cast<std::string>(value);
}

inline std::string encode_string(const decof::string &value)
{
    return std::string("\"") + value + "\"";
}

inline std::string encode_binary(const decof::binary &value)
{
    return std::string("&") + base64encode(value);
}

inline std::string encode_boolean_seq(decof::boolean_seq value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), encode_boolean);
    return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
}

inline std::string encode_integer_seq(decof::integer_seq value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), encode_integer);
    return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
}

inline std::string encode_real_seq(decof::real_seq value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), encode_real);
    return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
}

inline std::string encode_string_seq(const decof::string_seq &value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), encode_string);
    return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
}

inline std::string encode_binary_seq(const decof::binary_seq &value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), encode_binary);
    return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
}

inline std::string encode_tuple(const decof::dynamic_tuple &value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), decof::string_encoder::encode);
    return std::string("{") + boost::algorithm::join(str_vec, ",") + "}";
}

} // Anonymous namespace

namespace decof
{

std::string string_encoder::encode(const boost::any &any_value)
{
    if (any_value.type() == typeid(boolean))
        return encode_boolean(boost::any_cast<boolean>(any_value));
    else if (any_value.type() == typeid(integer))
        return encode_integer(boost::any_cast<integer>(any_value));
    else if (any_value.type() == typeid(real))
        return encode_real(boost::any_cast<real>(any_value));
    else if (any_value.type() == typeid(string))
        return encode_string(boost::any_cast<string>(any_value));
    else if (any_value.type() == typeid(binary))
        return encode_binary(boost::any_cast<binary>(any_value));
    else if (any_value.type() == typeid(boolean_seq))
        return encode_boolean_seq(boost::any_cast<boolean_seq>(any_value));
    else if (any_value.type() == typeid(integer_seq))
        return encode_integer_seq(boost::any_cast<integer_seq>(any_value));
    else if (any_value.type() == typeid(real_seq))
        return encode_real_seq(boost::any_cast<real_seq>(any_value));
    else if (any_value.type() == typeid(string_seq))
        return encode_string_seq(boost::any_cast<string_seq>(any_value));
    else if (any_value.type() == typeid(binary_seq))
        return encode_binary_seq(boost::any_cast<binary_seq>(any_value));
    else if (any_value.type() == typeid(dynamic_tuple))
        return encode_tuple(boost::any_cast<dynamic_tuple>(any_value));
    else
        throw wrong_type_error();
}

} // namespace decof
