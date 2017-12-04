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

#include "encoder.h"

#include <map>
#include <ostream>
#include <iomanip>

#include <boost/algorithm/string/join.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <decof/conversion.h>
#include <decof/exceptions.h>
#include <decof/types.h>

namespace {

/// Base64 string encoder function.
std::string base64_encode(const decof::binary &bin)
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

} // Anonymous namespace

namespace decof
{

namespace cli
{

//void encoder::encode_binary(std::ostream &out, const binary &value)
//{
//    out << "&" << base64_encode(value);
//}

encoder::encoder(std::ostream& out) :
    m_out(out)
{}

void encoder::operator()(const scalar_t& arg) const
{
    boost::apply_visitor(*this, arg);
}

void encoder::operator()(const sequence_t& arg) const
{
    m_out << '[';

    auto it = std::cbegin(arg.value);
    for (; it != std::cend(arg.value); ++it) {
        if (it != std::cbegin(arg.value)) m_out.put(',');
        boost::apply_visitor(*this, *it);
    }

    m_out << ']';
}

void encoder::operator()(const tuple_t& arg) const
{
    m_out << '{';

    auto it = std::cbegin(arg.value);
    for (; it != std::cend(arg.value); ++it) {
        if (it != std::cbegin(arg.value)) m_out.put(',');
        boost::apply_visitor(*this, *it);
    }

    m_out << '}';
}

void encoder::operator()(const boolean& arg) const
{
    m_out << (arg ? "#t" : "#f");
}

void encoder::operator()(const integer& arg) const
{
    m_out << arg;
}

void encoder::operator()(const real& arg) const
{
    // Use minimum value without loss of precision in conversion between binary
    // and decimal number representation and vice versa.
    m_out << std::setprecision(17) << arg;
}

void encoder::operator()(const string& arg) const
{
    static const std::map<char, char> escape_characters = {
        { '\a', 'a' },
        { '\b', 'b' },
        { '\f', 'f' },
        { '\n', 'n' },
        { '\r', 'r' },
        { '\t', 't' },
        { '\v', 'v' },
        { '\\', '\\' },
        { '\'', '\'' },
        { '\"', '"' },
        { '\?', '?' }
    };

    m_out << '"';

    for (const auto& ch : arg) {
        auto it = escape_characters.cbegin();

        if (ch >= 0x20 && ch <= 0x7F && ch != '\\' && ch != '\'' && ch != '\"' && ch != '\?') {
            m_out << ch;
        } else if ((it = escape_characters.find(ch)) != escape_characters.cend()) {
            m_out << "\\" << it->second;
        } else {
            m_out << "\\x" << std::hex << ch;
        }
    }

    m_out << '"';
}

//void encoder::operator()(const sequence<integer>& val) const
//{
//    m_out << '[';

//    for (auto it = val.cbegin(); it != val.cend(); ++it) {
//        if (it != val.cbegin())
//            m_out << ",";
//        this->operator()(*it);
//    }

//    m_out << '[';
//}

//void encoder::operator()(const sequence<real>& val) const
//{
//    m_out << '[';

//    for (auto it = val.cbegin(); it != val.cend(); ++it) {
//        if (it != val.cbegin())
//            m_out << ",";
//        this->operator()(*it);
//    }

//    m_out << '[';
//}

//void encoder::operator()(const sequence<string>& val) const
//{
//    m_out << '[';

//    for (auto it = val.cbegin(); it != val.cend(); ++it) {
//        if (it != val.cbegin())
//            m_out << ",";
//        this->operator()(*it);
//    }

//    m_out << '[';
//}

} // namespace cli

} // namespace decof
