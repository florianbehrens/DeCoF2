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

#include "context.h"

#include <cassert>
#include <cctype>
#include <sstream>
#include <utility>

#include <boost/algorithm/string/trim.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include "array_view.h"
#include "bencode_string_parser.h"
#include "connection.h"
#include "endian.h"
#include "exceptions.h"
#include "js_value_encoder.h"
#include "xml_visitor.h"

namespace decof
{

namespace scgi
{

scgi_context::scgi_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection, userlevel_t userlevel) :
    client_context(a_object_dictionary, connection, userlevel)
{}

std::string scgi_context::connection_type() const
{
    return "scgi";
}

std::string scgi_context::remote_endpoint() const
{
    return remote_endpoint_;
}

void scgi_context::preload()
{
    // Connect to signals of connection class
    connection_->read_signal.connect(std::bind(&scgi_context::read_handler, this, std::placeholders::_1));
    connection_->async_read_some();
}

void scgi_context::read_handler(const std::string &cstr)
{
    try
    {
        auto result = parser_.parse(cstr.cbegin(), cstr.cend());
        if (result == request_parser::indeterminate) {
            connection_->async_read_some();
            return;
        } else if (result == request_parser::good) {
            remote_endpoint_ = std::string("tcp://") + parser_.headers.at("REMOTE_ADDR") + ":" + parser_.headers.at("REMOTE_PORT");

            if (parser_.method == request_parser::method_type::get)
                handle_get_request();
            else if (parser_.method == request_parser::method_type::put)
                handle_put_request();
            else if (parser_.method == request_parser::method_type::post)
                handle_post_request();
            else
                throw parse_error();
        } else
            throw parse_error();
    } catch (access_denied_error&) {
        send_response(response::stock_response(response::status_code::unauthorized));
    } catch (invalid_parameter_error&) {
        send_response(response::stock_response(response::status_code::not_found));
    } catch (runtime_error&) {
        send_response(response::stock_response(response::status_code::bad_request));
    } catch (std::out_of_range&) {
        send_response(response::stock_response(response::status_code::bad_request));
    } catch (...) {
        send_response(response::stock_response(response::status_code::internal_server_error));
    }

    connection_->disconnect();
}

void scgi_context::handle_get_request()
{
    std::ostringstream body_oss;
    response resp = response::stock_response(response::status_code::ok);

    if (parser_.uri == "/browse" || parser_.uri == "/browse/") {
        resp.headers["Content-Type"] = "text/xml";
        xml_visitor visitor(body_oss);
        browse(&visitor);
    } else {
        resp.headers["Content-Type"] = "text/plain";
        scgi::js_value_encoder().encode_any(body_oss, get_parameter(parser_.uri, '/'));
    }

    resp.body = std::move(body_oss.str());
    send_response(resp);
}

void scgi_context::handle_put_request()
{
    std::string str;
    boost::any value;
    std::vector<boost::any> vec;

    boost::algorithm::trim_if(parser_.body, boost::is_space());
    std::istringstream ss(parser_.body);

    try {
        if (parser_.content_type == "vnd/com.toptica.decof.boolean") {
            if (parser_.body == "true")
                value = true;
            else if (parser_.body == "false")
                value = false;
            else
                throw invalid_value_error();
        } else if (parser_.content_type == "vnd/com.toptica.decof.integer") {
            ss >> str;
            value = boost::lexical_cast<decof::integer>(str);
        } else if (parser_.content_type == "vnd/com.toptica.decof.real") {
            ss >> str;
            value = boost::lexical_cast<decof::real>(str);
        } else if (parser_.content_type == "vnd/com.toptica.decof.string") {
            value = std::move(parser_.body);
        } else if (parser_.content_type == "vnd/com.toptica.decof.binary") {
            decof::binary bin;
            bin.swap(parser_.body);
            value = std::move(bin);
        } else if (parser_.content_type == "vnd/com.toptica.decof.boolean_seq") {
            vec.reserve(parser_.body.size() / sizeof(char));
            for (char c : parser_.body)
                vec.emplace_back(boost::any(c > 0));
            value = std::move(vec);
        } else if (parser_.content_type == "vnd/com.toptica.decof.integer_seq") {
            if (parser_.body.size() % sizeof(decof::integer))
                throw invalid_value_error();

            size_t size = parser_.body.size() / sizeof(decof::integer);
            vec.reserve(size);
            array_view<const int32_t> elems(reinterpret_cast<const int32_t*>(&parser_.body[0]), size);
            for (auto elem : elems)
                vec.emplace_back(little_endian_to_native(elem));
            value = std::move(vec);
        } else if (parser_.content_type == "vnd/com.toptica.decof.real_seq") {
            if (parser_.body.size() % sizeof(decof::real))
                throw invalid_value_error();

            size_t size = parser_.body.size() / sizeof(decof::real);
            vec.reserve(size);

            array_view<const double> elems(reinterpret_cast<const double*>(&parser_.body[0]), size);
            for (auto elem : elems)
                vec.emplace_back(little_endian_to_native(elem));
            value = std::move(vec);
        } else if (parser_.content_type == "vnd/com.toptica.decof.string_seq") {
            auto it = parser_.body.cbegin();
            for (; it != parser_.body.cend(); it += 2) {
                bencode_string_parser parser;
                bencode_string_parser::result_type result;
                std::tie(result, it) = parser.parse(it, parser_.body.cend());

                if (result == bencode_string_parser::good) {
                    vec.push_back(boost::any(std::move(parser.data)));
                } else
                    throw invalid_value_error();

                if (it == parser_.body.cend())
                    break;
            }

            value = std::move(vec);
        } else if (parser_.content_type == "vnd/com.toptica.decof.binary_seq") {
            auto it = parser_.body.cbegin();
            for (; it != parser_.body.cend(); it += 2) {
                bencode_string_parser parser;
                bencode_string_parser::result_type result;
                std::tie(result, it) = parser.parse(it, parser_.body.cend());

                if (result == bencode_string_parser::good) {
                    decof::binary binary;
                    binary.swap(parser.data);
                    vec.push_back(boost::any(std::move(binary)));
                } else
                    throw invalid_value_error();

                if (it == parser_.body.cend())
                    break;
            }

            value = std::move(vec);
        } else if (parser_.content_type == "vnd/com.toptica.decof.tuple")
            throw not_implemented_error();
        else
            throw wrong_type_error();
    } catch (boost::bad_any_cast&) {
        throw invalid_value_error();
    } catch (boost::bad_lexical_cast&) {
        throw invalid_value_error();
    }

    set_parameter(parser_.uri, value, '/');
    send_response(response::stock_response(response::status_code::ok));
}

void scgi_context::handle_post_request()
{
    signal_event(parser_.uri, '/');
    send_response(response::stock_response(response::status_code::ok));
}

void scgi_context::send_response(const response &resp)
{
    std::stringstream ss;
    ss << resp;
    connection_->async_write(ss.str());
}

} // namespace decof

} // namespace scgi
