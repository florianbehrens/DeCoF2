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

#include "textproto_encoder.h"

#include <boost/any.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "conversion.h"
#include "exceptions.h"
#include "types.h"

namespace {

struct string_escaper
{
    template<typename T>
    std::string operator()(const T& match) const
    {
        std::string s;
        typename T::const_iterator i = match.begin();
        for (; i != match.end(); i++) {
            s += str(boost::format("%02x") % static_cast<int>(*i));
        }
        return s;
    }
};

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

} // Anonymous namespace

namespace decof
{

void textproto_encoder::encode(std::stringstream &out, const boolean &value)
{
    if (value == true)
        out << "#t";
    else
        out << "#f";
}

void textproto_encoder::encode(std::stringstream &out, const string &value)
{
    std::string escaped = value;
    boost::find_format_all(escaped, boost::token_finder(!boost::is_print() || boost::is_any_of("\"")), string_escaper());
    out << "\"" << escaped << "\"";
}

void textproto_encoder::encode(std::stringstream &out, const binary &value)
{
    out << "&" << base64encode(value);
}

} // namespace decof
