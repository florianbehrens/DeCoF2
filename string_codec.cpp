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

#include "string_codec.h"

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "container_types.h"
#include "exceptions.h"

namespace {

std::string bool2string(bool value)
{
    if (value == true)
        return std::string("#t");
    return std::string("#f");
}

} // Anonymous namespace

namespace decof
{

std::string string_codec::encode(const boost::any &any_value)
{
    string_vector str_vec;

    if (any_value.type() == typeid(bool)) {
        return bool2string(boost::any_cast<bool>(any_value));
    } else if (any_value.type() == typeid(int))
        return boost::lexical_cast<std::string>(boost::any_cast<int>(any_value));
    else if (any_value.type() == typeid(double))
        return boost::lexical_cast<std::string>(boost::any_cast<double>(any_value));
    else if (any_value.type() == typeid(std::string))
        return std::string("\"") + boost::any_cast<std::string>(any_value) + "\"";
    else if (any_value.type() == typeid(bool_vector)) {
        bool_vector value = boost::any_cast<bool_vector>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), bool2string);
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else if (any_value.type() == typeid(int_vector)) {
        int_vector value = boost::any_cast<int_vector>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), [](int value) {
            return boost::lexical_cast<std::string>(value);
        });
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else if (any_value.type() == typeid(double_vector)) {
        double_vector value = boost::any_cast<double_vector>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), [](double value) {
            return boost::lexical_cast<std::string>(value);
        });
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else if (any_value.type() == typeid(string_vector))
        return std::string("[\"") + boost::algorithm::join(boost::any_cast<string_vector>(any_value), std::string("\",\"")) + "\"]";
    else
        throw wrong_type_error();
}

boost::any string_codec::decode(const std::string &cstr)
{
    assert(cstr.size() > 0);

    std::string str(cstr);
    boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v"));

    try {
        if (str.front() == '#') {
            // bool
            if (str == "#f")
                return boost::any(false);
            return boost::any(true);
        } else if (str.front() == '"' && str.back() == '"') {
            // Decode string
            boost::algorithm::trim_if(str, boost::is_any_of("\""));
            boost::replace_all(str, "\\\"", "\"");
            return boost::any(str);
        } else if (str.front() == '[' && str.back() == ']') {
            // Decode array while checking whether all elems have equal type
            boost::algorithm::trim_if(str, boost::is_any_of("[]"));

            std::vector<std::string> elems;
            boost::algorithm::split(elems, str, boost::algorithm::is_any_of(","), boost::algorithm::token_compress_off);

            boost::any last_any_elem;
            std::vector<boost::any> any_elems;
            for (auto elem : elems) {
                boost::any any_elem = decode(elem);
                if (!last_any_elem.empty() && last_any_elem.type() != any_elem.type())
                    throw wrong_type_error();
                any_elems.push_back(any_elem);
            }

            return boost::any(any_elems);
        } else if (boost::algorithm::all(str, boost::is_digit() || boost::is_any_of("-"))) {
            // possibly int
            return boost::any(boost::lexical_cast<int>(str));
        } else {
            // double
            return boost::any(boost::lexical_cast<double>(str));
        }
    } catch (boost::bad_lexical_cast& e) {
        throw wrong_type_error();
    }

    throw wrong_type_error();
}

} // namespace decof
