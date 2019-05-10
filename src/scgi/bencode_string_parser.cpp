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

#include "bencode_string_parser.h"
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <cctype>

namespace decof {

namespace scgi {

bencode_string_parser::bencode_string_parser()
{
    reset();
}

void bencode_string_parser::reset()
{
    data.clear();
    state_ = string_length;
    length_str_.clear();
}

bencode_string_parser::result_type bencode_string_parser::consume(char input) noexcept
{
    try {
        switch (state_) {
            case string_length:
                if (std::isdigit(input))
                    length_str_.push_back(input);
                else if (input == ':') {
                    length_ = boost::lexical_cast<size_t>(length_str_);
                    state_  = string;
                    if (length_ == 0)
                        return good;
                } else
                    return bad;
                break;
            case string:
                data.push_back(input);
                length_ -= 1;
                if (length_ == 0)
                    return good;
                break;
            default:
                assert(false);
        }
    } catch (...) {
        return bad;
    }

    return indeterminate;
}

} // namespace scgi

} // namespace decof
