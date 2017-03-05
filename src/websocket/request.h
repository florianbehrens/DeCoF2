/*
 * Copyright (c) 2017 Florian Behrens
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

#ifndef DECOF_WEBSOCKET_REQUEST_H
#define DECOF_WEBSOCKET_REQUEST_H

#include <iostream> // TODO
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

#include <boost/any.hpp>
#include <boost/blank.hpp>
#include <boost/variant/variant.hpp>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h> // TODO

#include <decof/exceptions.h>

#include "errors.h"

namespace decof
{

namespace websocket
{

using id_type = boost::variant<boost::blank, bool, int64_t, double, std::string>;

/**
 * @brief A custom RapidJSON stream for use with a pair of (non-contiguous)
 * iterators.
 */
template<typename Begin, typename End>
class iterator_stream_wrapper
{
public:
    typedef char Ch;

    iterator_stream_wrapper(const Begin& begin, const End& end) :
        begin_(begin),
        end_(end)
    {}

    iterator_stream_wrapper(const iterator_stream_wrapper&) = delete;
    iterator_stream_wrapper& operator=(const iterator_stream_wrapper&) = delete;

    Ch Peek() const
    {
        if (begin_ == end_)
            return '\0';

        return static_cast<Ch>(*begin_);
    }

    Ch Take()
    {
        if (begin_ == end_)
            return '\0';

        auto ch = *begin_++;
        count_ += 1;

        return static_cast<Ch>(ch);
    }

    size_t Tell() const
    {
        return count_;
    }

    Ch* PutBegin()
    {
        assert(false);
        return 0;
    }

    void Put(Ch)
    {
        assert(false);
    }

    void Flush()
    {
        assert(false);
    }

    size_t PutEnd(Ch*)
    {
        assert(false);
        return 0;
    }

private:
    typename std::decay<const Begin>::type begin_;
    const End& end_;
    size_t count_ = 0;
};

class request
{
    template<typename Encoding>
    boost::any extract_scalar(const rapidjson::GenericValue<Encoding>& json_value)
    {
        switch (json_value.GetType()) {
        case rapidjson::kFalseType:
        case rapidjson::kTrueType:
            return json_value.GetBool();
        case rapidjson::kNumberType:
            if (json_value.IsInt()) {
                return json_value.GetInt();
            } else if (json_value.IsLosslessDouble()) {
                return json_value.GetDouble();
            }

            throw invalid_params_error();
        case rapidjson::kStringType:
            return std::string(json_value.GetString());
        default:
            throw invalid_params_error();
        }
    }

public:
    template<typename Begin, typename End>
    void parse(const Begin& begin, const End& end)
    {
        /*
         * JSON parser requirements:
         * - Commercial use allowed
         * - Non-contiguous iterator interface (RapidJSON with custom stream, no insitu parsing)
         * - Windows & Linux support
         */

        iterator_stream_wrapper<Begin, End> input(begin, end);

        rapidjson::Document document;
        if (! document.ParseStream(input).HasParseError() && document.IsObject()) {
            // If property 'jsonrpc' is present, it must be '2.0'
            rapidjson::Document::ConstMemberIterator it;
            if ((it = document.FindMember("jsonrpc")) != document.MemberEnd()) {
                if (it->value != "2.0")
                    throw bad_request_error();
            }

            if ((it = document.FindMember("method")) != document.MemberEnd()) {
                method = it->value.GetString();
            }

            if ((it = document.FindMember("params")) != document.MemberEnd()) {
                auto& params = it->value;

                if (params.IsArray()) {
                    if (params.Size() > 0) {
                        uri = params[0].GetString();
                    } else
                        throw bad_request_error();

                    if (method == "set") {
                        if (params.Size() != 2)
                            throw invalid_params_error();

                        const auto& json_value = params[1];

                        if (json_value.GetType() == rapidjson::kArrayType) {
                            std::vector<boost::any> tmp_value;
                            tmp_value.reserve(json_value.Size());

                            for (const auto& elem : json_value.GetArray())
                                tmp_value.emplace_back(extract_scalar(elem));

                            value = std::move(tmp_value);
                        } else {
                            value = extract_scalar(json_value);
                        }
                    }
                } else if (params.IsObject()) {
                    // TODO
                } else
                    throw bad_request_error();
            }

            if ((it = document.FindMember("id")) != document.MemberEnd() && it->value.IsUint64()) {
                id = int64_t(it->value.GetInt());
            } else
                throw bad_request_error();
        } else {
            // TODO
            rapidjson::ParseResult ok = document;
            std::cerr << rapidjson::GetParseError_En(ok.Code()) << "(" << ok.Offset() << ")" << std::endl;
            throw parse_error();
        }
    }

    bool has_id() const;

    std::string method;
    std::string uri;
    boost::any value;
    id_type id;
};

/**
 * @brief Insertion operator for JSON-RPC request.
 *
 * The following is an example of a (pretty printed) serialized JSON request
 * object:
 *
 * @code
 * {
 *     jsonrpc: "2.0",                      (optional)
 *     method: "set",                       (or get, subscribe, unsubscribe, publish)
 *     params: [ "laser1:enable", true ],   (only when method == set)
 *     id: 123                              (any datatype possible)
 * }
 * @endcode
 */
std::ostream& operator<<(std::ostream& out, const request& r);

} // namespace websocket

} // namespace decof

#endif // DECOF_WEBSOCKET_REQUEST_H
