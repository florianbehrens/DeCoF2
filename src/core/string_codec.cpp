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
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/lexical_cast.hpp>

#include "types.h"
#include "exceptions.h"

namespace {

std::string bool2string(bool value)
{
    if (value == true)
        return std::string("#t");
    return std::string("#f");
}

// The following two functions are based on code from
// http://stackoverflow.com/questions/10521581/base64-encode-using-boost-throw-exception/10973348#10973348
std::string base64encode(decof::binary bin)
{
    using namespace boost::archive::iterators;

    typedef base64_from_binary<transform_width<std::string::const_iterator, 6, 8>> it_base64_t;

    unsigned int writePaddChars = (3 - bin.length() % 3) % 3;
    std::string base64(it_base64_t(bin.begin()), it_base64_t(bin.end()));
    base64.append(writePaddChars, '=');

    return base64;
}

decof::binary base64decode(std::string base64)
{
    using namespace boost::archive::iterators;

    typedef transform_width<binary_from_base64<remove_whitespace<std::string::const_iterator>>, 8, 6> it_binary_t;

    unsigned int paddChars = count(base64.begin(), base64.end(), '=');
    std::replace(base64.begin(), base64.end(), '=', 'A');
    decof::binary result(it_binary_t(base64.begin()), it_binary_t(base64.end()));
    result.erase(result.end() - paddChars, result.end());
    return result;
}

} // Anonymous namespace

namespace decof
{

std::string string_codec::encode(const boost::any &any_value)
{
    string_seq str_vec;

    if (any_value.type() == typeid(boolean)) {
        return bool2string(boost::any_cast<bool>(any_value));
    } else if (any_value.type() == typeid(integer))
        return boost::lexical_cast<std::string>(boost::any_cast<int>(any_value));
    else if (any_value.type() == typeid(real))
        return boost::lexical_cast<std::string>(boost::any_cast<double>(any_value));
    else if (any_value.type() == typeid(string))
        return std::string("\"") + boost::any_cast<std::string>(any_value) + "\"";
    else if (any_value.type() == typeid(binary))
        return std::string("&") + base64encode(boost::any_cast<binary>(any_value));
    else if (any_value.type() == typeid(boolean_seq)) {
        boolean_seq value = boost::any_cast<boolean_seq>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), bool2string);
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else if (any_value.type() == typeid(integer_seq)) {
        integer_seq value = boost::any_cast<integer_seq>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), encode);
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else if (any_value.type() == typeid(real_seq)) {
        real_seq value = boost::any_cast<real_seq>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), encode);
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else if (any_value.type() == typeid(string_seq)) {
        string_seq value = boost::any_cast<string_seq>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), encode);
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else if (any_value.type() == typeid(binary_seq)) {
        binary_seq value = boost::any_cast<binary_seq>(any_value);
        str_vec.resize(value.size());
        std::transform(value.begin(), value.end(), str_vec.begin(), encode);
        return std::string("[") + boost::algorithm::join(str_vec, ",") + "]";
    } else
        throw wrong_type_error();
}

boost::any string_codec::decode(const std::string &cstr)
{
    assert(cstr.size() > 0);

    std::string str(cstr);
    boost::algorithm::trim_if(str, boost::is_any_of(" \f\n\r\t\v"));

    try {
        if (str.front() == '#') {
            // boolean
            if (str == "#f")
                return boost::any(boolean(false));
            return boost::any(boolean(true));
        } else if (str.front() == '&') {
            // binary
            try {
                return boost::any(binary(base64decode(str.substr(1))));
            } catch (std::exception&) {
                throw invalid_value_error();
            }
        } else if (str.front() == '"' && str.back() == '"') {
            // string
            boost::algorithm::trim_if(str, boost::is_any_of("\""));
            boost::replace_all(str, "\\\"", "\"");
            return boost::any(string(str));
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
            // possibly integer
            return boost::any(boost::lexical_cast<integer>(str));
        } else {
            // real
            return boost::any(boost::lexical_cast<real>(str));
        }
    } catch (boost::bad_lexical_cast&) {
        throw wrong_type_error();
    }

    throw wrong_type_error();
}

} // namespace decof
