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

#ifndef DECOF_SCGI_PARSER_H
#define DECOF_SCGI_PARSER_H

#include <map>
#include <string>

namespace decof
{

namespace scgi
{

class request_parser
{
public:
    typedef std::map<std::string, std::string> headers_type;

    enum method_type
    {
        get,
        post,
        head,
        put,
        delete_,
        trace,
        options,
        connect,
        undefined
    };

    /// Result of parse.
    enum result_type { good, bad, indeterminate };

    request_parser();

    /// Reset to initial parser state.
    void reset();

    /// @brief Start parsing procedure.
    ///
    /// @returns #good when a complete request has been parsed, #bad if the
    /// data is invalid, #indeterminate when more data is required.
    template <typename InputIterator>
    result_type parse(InputIterator begin, InputIterator end) noexcept
    {
        result_type result;
        while (begin != end)
        {
            result = consume(*begin++);
            if (result == bad)
                return bad;
        }
        return result;
    }

    method_type method;
    std::string uri;
    headers_type headers;
    std::string body;

private:
    /// Handle the next character of input.
    result_type consume(char input) noexcept;

    enum state
    {
        netstring_length,
        header_start,
        header_name,
        header_value,
        body_range
    } state_;

    std::string netstring_length_str_;
    size_t netstring_length_;
    size_t netstring_count_;
    std::string current_header_;
    std::string current_value_;
    size_t content_length_;
};

} // namespace decof

} // namespace scgi

#endif // DECOF_SCGI_PARSER_H
