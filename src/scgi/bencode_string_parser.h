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

#ifndef DECOF_SCGI_BENCODE_STRING_PARSER_H
#define DECOF_SCGI_BENCODE_STRING_PARSER_H

#include <string>
#include <tuple>

namespace decof
{

namespace scgi
{

/// @brief Bencode string parser.
///
/// See the bencode string specification at http://en.wikipedia.org/wiki/Bencode.
class bencode_string_parser
{
public:
    /// Result of parse.
    enum result_type { good, bad, indeterminate };

    bencode_string_parser();

    /// Reset to initial parser state.
    void reset();

    /// @brief Start parsing procedure.
    ///
    /// @returns #good when a complete value has been parsed, #bad if the
    /// data is invalid, #indeterminate when more data is required.
    template<typename InputIterator>
    std::tuple<result_type, InputIterator> parse(InputIterator begin, InputIterator end) noexcept
    {
        result_type result = indeterminate;
        while (result == indeterminate && begin != end)
            result = consume(*begin++);

        return std::make_tuple(result, begin);
    }

    /// The parsed string data.
    std::string data;

private:
    /// Handle the next character of input.
    result_type consume(char input) noexcept;

    enum state
    {
        string_length,
        string
    } state_;

    std::string length_str_;
    size_t length_;
};

} // namespace scgi

} // namespace decof

#endif // DECOF_SCGI_BENCODE_STRING_PARSER_H
