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

#ifndef DECOF_HTTP_BODY_PARSER_H
#define DECOF_HTTP_BODY_PARSER_H

#include <string>

//#include <boost/proto/detail/preprocessed/expr_variadic.hpp>

//#include <boost/proto/detail/expr.hpp>

//#include <boost/function_types/detail/pp_cc_loop/preprocessed.hpp>

namespace decof
{

class http_body_parser
{
public:
    /// Result of parse.
    enum result_type { good, bad, indeterminate };

    http_body_parser();

    /// Reset to initial parser state.
    void reset();

    /// Parsing result.
    result_type result() const;

    /// Parsed data.
    std::string data() const;

    /// Parse some data. The enum return value is good when a complete request has
    /// been parsed, bad if the data is invalid, indeterminate when more data is
    /// required. The InputIterator return value indicates how much of the input
    /// has been consumed.
    template <typename InputIterator>
    InputIterator parse(InputIterator begin, InputIterator end)
    {
        while (begin != end)
        {
            result_ = consume(*begin++);
            if (result_ == good || result_ == bad)
                return begin;
        }
        return begin;
    }

private:
    /// Handle the next character of input.
    result_type consume(char input);

    enum state
    {
        chunk_length,
        chunk_length_newline,
        chunk_data,
        chunk_data_newline
    } state_;

    result_type result_;
    size_t chunk_length_;
    size_t chunk_read_;
    std::string chunk_length_str_;
    std::string data_;
};

} // namespace decof

#endif // DECOF_HTTP_BODY_PARSER_H
