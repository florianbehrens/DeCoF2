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

#include "webservice_encoder.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "conversion.h"
#include "exceptions.h"
#include "types.h"

namespace
{

inline std::string encode_boolean(decof::boolean value)
{
    if (value == true)
        return std::string("true");
    return std::string("false");
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
    return value;
}

inline std::string encode_binary(const decof::binary &value)
{
    return value;
}

template<typename T>
inline std::string encode_sequence(decof::sequence<T> value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), decof::webservice_encoder::encode);
    return boost::algorithm::join(str_vec, ",");
}

inline std::string encode_tuple(const decof::dynamic_tuple &value)
{
    std::vector<std::string> str_vec(value.size());
    std::transform(value.begin(), value.end(), str_vec.begin(), decof::webservice_encoder::encode);
    return boost::algorithm::join(str_vec, ",");
}

} // Anonymous namespace

namespace decof
{

std::string webservice_encoder::encode(const boost::any &any_value)
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
        return encode_sequence(boost::any_cast<boolean_seq>(any_value));
    else if (any_value.type() == typeid(integer_seq))
        return encode_sequence(boost::any_cast<integer_seq>(any_value));
    else if (any_value.type() == typeid(real_seq))
        return encode_sequence(boost::any_cast<real_seq>(any_value));
    else if (any_value.type() == typeid(string_seq))
        return encode_sequence(boost::any_cast<string_seq>(any_value));
    else if (any_value.type() == typeid(binary_seq))
        return encode_sequence(boost::any_cast<binary_seq>(any_value));
    else if (any_value.type() == typeid(dynamic_tuple))
        return encode_tuple(boost::any_cast<dynamic_tuple>(any_value));
    else
        throw wrong_type_error();
}

} // namespace decof
