/*
 * Copyright (c) 2016 Florian Behrens
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

#ifndef DECOF_CLI_DECODER_H
#define DECOF_CLI_DECODER_H

#include <cctype>
#include <map>
#include <sstream>
#include <string>

#include <decof/exceptions.h>
#include <decof/types.h>

namespace decof
{

namespace cli
{

/// Base64 string decoder function.
std::string base64decode(std::string base64);

/**
 * @brief Converts the backslash escaped character sequence [#begin, #end) to an
 * unescaped character sequence starting at #out.
 *
 * @param begin Start of input character sequence.
 * @param end Past the end of the input character sequence.
 * @param out Output character sequence iterator.
 * @throw @a parse_error in case of an invalid input sequence.
 * @note The output character sequence must have the size of the input
 * character sequence unless a @a std::back_insert_iterator or similar is used.
 */
template<typename BeginIt, typename EndIt, typename OutIt>
size_t backslash_escape_decoder(const BeginIt& begin, const EndIt& end, OutIt out) try
{
    size_t out_count = 0;

    enum {
        none,       //< 'Ordinary' character
        backslash,  //< Backslash detected
        hex_digit1, //< First hex digit of hexadecimal escape sequence (e.g. \xAB)
        hex_digit2  //< 2nd hex digit
    } state = none;

    static const std::map<char, char> escape_characters = {
        { 'a', '\a' },
        { 'b', '\b' },
        { 'f', '\f' },
        { 'n', '\n' },
        { 'r', '\r' },
        { 't', '\t' },
        { 'v', '\v' },
        { '\\', '\\' },
        { '\'', '\'' },
        { '"', '\"' },
        { '?', '\?' }
    };

    auto ishexdigit = [](char c) -> bool {
        return std::isdigit(c) || (std::toupper(c) >= 'A' && std::toupper(c) <= 'F');
    };

    std::stringstream hex;

    for (auto it = begin; it != end; ++it) {
        char ch = *it;

        switch (state) {
        case none:
            if (ch == '\\')
                state = backslash;
            else if (ch < 0x20 || ch > 0x7F)
                throw parse_error();
            else {
                (*out++) = ch;
                out_count += 1;
            }
            break;
        case backslash:
            if (std::toupper(ch) == 'X')
                state = hex_digit1;
            else {
                // Throws std::out_of_range in case of invalid escape sequence:
                (*out++) = escape_characters.at(ch);
                out_count += 1;
                state = none;
            }
            break;
        case hex_digit1:
            if (ishexdigit(ch)) {
                hex.put(ch);
                state = hex_digit2;
            } else
                throw parse_error();
            break;
        case hex_digit2:
            if (ishexdigit(ch)) {
                hex.put(ch);

                int n;
                hex >> std::hex >> n;
                (*out++) = n;
                out_count += 1;

                std::stringstream tmp;
                hex.swap(tmp);

                state = none;
            } else
                throw parse_error();
            break;
        }
    }

    if (state != none)
        throw parse_error();

    return out_count;
} catch(...) {
    throw parse_error();
}

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_DECODER_H
