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

#include <decof/scgi/scgi_context.h>

#include <ostream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <decof/exceptions.h>
#include <decof/object_dictionary.h>

#include "array_view.h"
#include "bencode_string_parser.h"
#include "endian.h"
#include "js_value_encoder.h"
#include "xml_visitor.h"

namespace decof
{

namespace scgi
{

scgi_context::scgi_context(boost::asio::ip::tcp::socket&& socket, object_dictionary& od, userlevel_t userlevel) :
    client_context(od, userlevel),
    socket_(std::move(socket))
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
    auto self(std::dynamic_pointer_cast<scgi_context>(shared_from_this()));
    socket_.async_read_some(boost::asio::buffer(inbuf_),
                            std::bind(&scgi_context::read_handler, self,
                                      std::placeholders::_1, std::placeholders::_2));
}

void scgi_context::read_handler(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error) {
        try {
            auto result = parser_.parse(inbuf_.data(), inbuf_.data() + bytes_transferred);

            if (result == request_parser::good) {
                remote_endpoint_ = std::string("scgi://") + parser_.headers.at("REMOTE_ADDR") + ":" + parser_.headers.at("REMOTE_PORT");

                if (parser_.method == request_parser::method_type::get)
                    handle_get_request();
                else if (parser_.method == request_parser::method_type::put)
                    handle_put_request();
                else if (parser_.method == request_parser::method_type::post)
                    handle_post_request();
                else
                    throw parse_error();
            } else if (result == request_parser::bad)
                throw parse_error();
            else
                preload();
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
    } else
        disconnect();
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

        auto const& value = get_parameter(parser_.uri, '/');
        boost::apply_visitor(js_value_encoder(body_oss), value);
    }

    resp.body = std::move(body_oss.str());
    send_response(resp);
}

void scgi_context::handle_put_request()
{
    std::string str;
    value_t val;
    sequence_t seq;

    boost::algorithm::trim_if(parser_.body, boost::is_space());
    std::istringstream ss(parser_.body);

    try {
        if (parser_.content_type == "vnd/com.toptica.decof.boolean") {
            if (parser_.body == "true")
                val = true;
            else if (parser_.body == "false")
                val = false;
            else
                throw invalid_value_error();
        } else if (parser_.content_type == "vnd/com.toptica.decof.integer") {
            ss >> str;
            val = generic_scalar{ boost::lexical_cast<integer>(str) };
        } else if (parser_.content_type == "vnd/com.toptica.decof.real") {
            ss >> str;
            val = generic_scalar{ boost::lexical_cast<real>(str) };
        } else if (parser_.content_type == "vnd/com.toptica.decof.string") {
            val = generic_scalar{ std::move(parser_.body) };
        } else if (parser_.content_type == "vnd/com.toptica.decof.boolean_seq") {
//            seq.reserve(parser_.body.size() / sizeof(char));
            for (char c : parser_.body)
                seq.value.emplace_back(c > 0);
            val = std::move(seq);
        } else if (parser_.content_type == "vnd/com.toptica.decof.integer_seq") {
            if (parser_.body.size() % sizeof(integer))
                throw invalid_value_error();

            size_t size = parser_.body.size() / sizeof(integer);
//            seq.reserve(size);
            array_view<const integer> elems(reinterpret_cast<const integer*>(&parser_.body[0]), size);
            for (auto elem : elems)
                seq.value.emplace_back(static_cast<integer>(little_endian_to_native(elem)));
            val = std::move(seq);
        } else if (parser_.content_type == "vnd/com.toptica.decof.real_seq") {
            if (parser_.body.size() % sizeof(decof::real))
                throw invalid_value_error();

            size_t size = parser_.body.size() / sizeof(decof::real);
//            seq.reserve(size);

            array_view<const double> elems(reinterpret_cast<const double*>(&parser_.body[0]), size);
            for (auto elem : elems)
                seq.value.emplace_back(little_endian_to_native(elem));
            val = std::move(seq);
        } else if (parser_.content_type == "vnd/com.toptica.decof.string_seq") {
            auto it = parser_.body.cbegin();
            for (; it != parser_.body.cend(); it += 2) {
                bencode_string_parser parser;
                bencode_string_parser::result_type result;
                std::tie(result, it) = parser.parse(it, parser_.body.cend());

                if (result == bencode_string_parser::good) {
                    seq.value.push_back(generic_scalar{ std::move(parser.data) });
                } else
                    throw invalid_value_error();

                if (it == parser_.body.cend())
                    break;
            }

            val = std::move(seq);
        } else if (parser_.content_type == "vnd/com.toptica.decof.tuple")
            throw not_implemented_error();
        else
            throw wrong_type_error();
    } catch (boost::bad_lexical_cast&) {
        throw invalid_value_error();
    }

    set_parameter(parser_.uri, val, '/');
    send_response(response::stock_response(response::status_code::ok));
}

void scgi_context::handle_post_request()
{
    signal_event(parser_.uri, '/');
    send_response(response::stock_response(response::status_code::ok));
}

void scgi_context::send_response(const response &resp)
{
    std::ostream out(&outbuf_);
    out << resp;

    auto self(std::dynamic_pointer_cast<scgi_context>(shared_from_this()));
    boost::asio::async_write(socket_, outbuf_,
                             std::bind(&scgi_context::write_handler, self,
                                       std::placeholders::_1, std::placeholders::_2));
}

void scgi_context::write_handler(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error)
        preload();
    else
        disconnect();
}

void scgi_context::disconnect()
{
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);

    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto sptr = shared_from_this();
    object_dictionary_.remove_context(sptr);
}

} // namespace decof

} // namespace scgi
