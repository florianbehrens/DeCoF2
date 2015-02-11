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

#include "http_body_parser.h"

#include <boost/lexical_cast.hpp>

namespace decof
{

http_body_parser::http_body_parser()
{
    reset();
}

void http_body_parser::reset()
{
    state_ = chunk_length;
    result_ = indeterminate;
    data_.clear();
}

http_body_parser::result_type http_body_parser::result() const
{
    return result_;
}

std::string http_body_parser::data() const
{
    return data_;
}

http_body_parser::result_type http_body_parser::consume(char input)
{
    switch (state_)
    {
    case chunk_length:
        if (std::isdigit(input))
            chunk_length_str_.push_back(input);
        else if (input == '\r')
            state_ = chunk_length_newline;
        else
            return bad;
        break;
    case chunk_length_newline:
        if (input == '\n') {
            state_ = chunk_data;
            chunk_length_ = boost::lexical_cast<size_t>(chunk_length_str_);
            chunk_read_ = 0;
            chunk_length_str_.clear();
        } else
            return bad;
        break;
    case chunk_data:
        if (chunk_read_++ < chunk_length_)
            data_.push_back(input);
        else if (input == '\r')
            state_ = chunk_data_newline;
        else
            return bad;
        break;
    case chunk_data_newline:
        if (input == '\n') {
            if (chunk_length_ == 0)
                return good;
            else
                state_ = chunk_length;
        }
        else
            return bad;
        break;
    }

    return indeterminate;
}

} // namespace decof
