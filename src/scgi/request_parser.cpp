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

#include "request_parser.h"

#include <boost/lexical_cast.hpp>

namespace decof
{

namespace scgi
{

request_parser::request_parser()
{
    reset();
}

void request_parser::reset()
{
    state_ = netstring_length;
    method = method_type::undefined;
    uri.clear();
    headers.clear();
    body.clear();
    netstring_length_str_.clear();
    current_header_.clear();
    current_value_.clear();
    content_length_ = 0;
}

request_parser::result_type request_parser::consume(char input) noexcept
{
    try
    {
        netstring_count_ += 1;

        switch (state_)
        {
        case netstring_length:
            if (std::isdigit(input))
                netstring_length_str_.push_back(input);
            else if (input == ':') {
                netstring_length_ = boost::lexical_cast<size_t>(netstring_length_str_);
                netstring_count_ = 0;
                state_ = header_start;
            } else
                return bad;
            break;
        case header_start:
        case header_name:
            if (state_ == header_start && input == ',') {
                if (netstring_length_ == netstring_count_ - 1) {
                    if (content_length_ > 0)
                        state_ = body_range;
                    else
                        return good;
                } else
                    return bad;
            } else if (netstring_count_ < netstring_length_) {
                if (input == '\0') {
                    if (current_header_.size() > 0)
                        state_ = header_value;
                    else
                        return bad;
                } else
                    current_header_.push_back(input);
            } else
                return bad;
            break;
        case header_value:
            if (netstring_count_ <= netstring_length_) {
                if (input == '\0') {
                    // Check whether first header is CONTENT_LENGTH
                    if (headers.empty()) {
                        if (current_header_ == "CONTENT_LENGTH")
                            content_length_ = boost::lexical_cast<size_t>(current_value_);
                        else
                            return bad;
                    }

                    // Check if 2nd header is SCGI
                    if (headers.size() == 1 && !(current_header_ == "SCGI" && current_value_ == "1"))
                        return bad;

                    if (current_header_ == "REQUEST_METHOD") {
                        if (current_value_ == "GET")
                            method = method_type::get;
                        else if (current_value_ == "PUT")
                            method = method_type::put;
                        else if (current_value_ == "POST")
                            method = method_type::post;
                    }

                    if (current_header_ == "REQUEST_URI") {
                        uri = current_value_;
                    }

                    headers[current_header_].swap(current_value_);
                    current_header_.clear();
                    current_value_.clear();

                    state_ = header_start;
                } else
                    current_value_.push_back(input);
            } else
                return bad;
            break;
        case body_range:
            if (body.size() <= content_length_) {
                body.push_back(input);

                if (body.size() == content_length_)
                    return good;
            } else
                return bad;
            break;
        }
    } catch (boost::bad_lexical_cast) {
        return bad;
    } catch (...) {
        return bad;
    }

    return indeterminate;
}

} // namespace decof

} // namespace scgi
