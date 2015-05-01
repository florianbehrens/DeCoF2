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
#include <utility>

#include <boost/algorithm/string/trim.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include "connection.h"
#include "exceptions.h"
#include "js_value_encoder.h"

namespace
{

/*** @brief Static array container without memory ownership.
 * This container provides the standard container interface but does not hold
 * ownership of the underlying memory. */
template<typename T>
struct array_view
{
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;

    /// Constructs #array_view from pointer with size.
    explicit array_view(T *const ptr, size_t size) :
        ptr_(ptr),
        size_(size)
    {}

    /// Constructs #array_view from begin and end pointer.
    explicit array_view(T *const begin, T *const end) :
        ptr_(begin),
        size_(end - begin)
    {
        assert(end >= begin);
    }

    T& at(size_t pos) {
        if (pos >= size_)
            throw std::range_error("array_view: out of range access");
        return ptr_[pos];
    }

    const T& at(size_t pos) const {
        if (pos >= size_)
            throw std::range_error("array_view: out of range access");
        return ptr_[pos];
    }

    T& operator=(size_t pos) {
        return ptr_[pos];
    }

    const T& operator=(size_t pos) const {
        return ptr_[pos];
    }

    T& front() {
        return ptr_[0];
    }

    const T& front() const {
        return ptr_[0];
    }

    T& back() {
        return ptr_[size_-1];
    }

    const T& back() const {
        return ptr_[size_-1];
    }

    T* data() {
        return ptr_;
    }

    const T* data() const {
        return ptr_;
    }

    iterator begin() {
        return ptr_;
    }

    const_iterator begin() const {
        return ptr_;
    }

    const_iterator cbegin() const {
        return ptr_;
    }

    iterator end() {
        return ptr_ + size_;
    }

    const_iterator end() const {
        return ptr_ + size_;
    }

    const_iterator cend() const {
        return ptr_ + size_;
    }

    size_t size() const {
        return size_;
    }

private:
    T *const ptr_;
    const size_t size_;
};

} // Anonymous namespace

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
    } catch (...) {
        send_response(response::stock_response(response::status_code::internal_server_error));
    }

    connection_->disconnect();
}

void scgi_context::handle_get_request()
{
    boost::any any_value = get_parameter(parser_.uri, '/');
    response resp = response::stock_response(response::status_code::ok);

    std::stringstream body_ss;
    scgi::js_value_encoder().encode_any(body_ss, any_value);
    resp.headers["Content-Type"] = "text/plain";
    resp.body = std::move(body_ss.str());

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
            value = parser_.body;
        } else if (parser_.content_type == "vnd/com.toptica.decof.binary") {
            decof::binary bin;
            bin.swap(parser_.body);
            value = std::move(bin);
        } else if (parser_.content_type == "vnd/com.toptica.decof.boolean_seq") {
            vec.reserve(parser_.body.size() / sizeof(char));
            std::transform(parser_.body.cbegin(),parser_.body.cend(), vec.begin(), [](const char c) { return c > 0; });
            value = vec;
        } else if (parser_.content_type == "vnd/com.toptica.decof.integer_seq") {
            size_t size = parser_.body.size() / sizeof(decof::integer);
            vec.reserve(size);
            int32_t *elems = reinterpret_cast<int32_t*>(&parser_.body[0]);
            std::copy_n(elems, size, vec.begin());
            value = vec;
        } else if (parser_.content_type == "vnd/com.toptica.decof.real_seq") {
            size_t size = parser_.body.size() / sizeof(decof::real);
            vec.reserve(size);
            double *elems = reinterpret_cast<double*>(&parser_.body[0]);
            std::copy_n(elems, size, vec.begin());
            value = vec;
        } else if (parser_.content_type == "vnd/com.toptica.decof.string_seq") {
            auto pos = parser_.body.find("\r\n");
            if (pos >= parser_.body.size())
                throw invalid_value_error();

            array_view<const int32_t> view(reinterpret_cast<const int32_t*>(parser_.body.data()), pos / sizeof(int32_t));
            vec.resize(view.size());

            pos += 2;
            for (const auto &size : view) {
                if (pos + size > parser_.body.size())
                    throw invalid_value_error();
                str = parser_.body.substr(pos, pos + size);
                vec.push_back(boost::any(str));
            }
        } else if (parser_.content_type == "vnd/com.toptica.decof.binary_seq") {
            throw std::runtime_error("Not yet implemented");
        } else if (parser_.content_type == "vnd/com.toptica.decof.tuple")
            throw std::runtime_error("Not yet implemented");
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
