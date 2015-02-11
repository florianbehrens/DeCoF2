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

#include "http_context.h"

#include <memory>

#include <boost/algorithm/string/trim.hpp>

#include "exceptions.h"
#include "json_visitor.h"
#include "object_dictionary.h"
#include "webservice_encoder.h"
#include "xml_visitor.h"

namespace {

std::string get_header(const std::vector<boost::network::http::request_header_narrow> &headers, const std::string &name)
{
    for (auto &header : headers) {
        if (boost::to_lower_copy(header.name) == boost::to_lower_copy(name))
            return header.value;
    }

    return std::string();
}

} // Anonymous namespace

namespace decof
{

http_context::http_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection, userlevel_t userlevel) :
    client_context(a_object_dictionary, connection, userlevel),
    io_service_ptr_(a_object_dictionary.io_service()),
    options_(*this)
{}

std::string http_context::connection_type() const
{
    return "http";
}

std::string http_context::remote_endpoint() const
{
    return remote_endpoint_;
}

void http_context::preload()
{
    // Create boost::shared_ptr from std::shared_ptr with custom, empty deleter
    boost::shared_ptr<boost::asio::io_service> io_service_bptr(object_dictionary_.io_service().get(), [](void const *) {});
    http_server_ptr_.reset(new http_server(options_.io_service(io_service_bptr).address("127.0.0.1").port("8081")));
    http_server_ptr_->listen();
}

void http_context::operator()(const http_server::request &request, http_server::connection_ptr connection)
{
    std::exception_ptr ex_ptr;
    try {
        std::string transfer_encoding = get_header(request.headers, "Transfer-Encoding");

        if (transfer_encoding == std::string("chunked")) {
            read_chunked_body(request, connection);
        } else {
            std::string content_length_str = get_header(request.headers, "Content-Length");
            size_t content_length = 0;

            if (!content_length_str.empty())
                content_length = boost::lexical_cast<size_t>(content_length);

            read_body(request, connection, content_length);
        }
    } catch (...) {
        ex_ptr = std::current_exception();
    }

    if (ex_ptr)
        handle_exception(connection, ex_ptr);
}

void http_context::read_body(const http_server::request &request, http_server::connection_ptr connection, size_t bytes_to_read)
{
    if (bytes_to_read > 0) {
        connection->read(
            boost::bind(
                &http_context::read_body_handler,
                std::static_pointer_cast<http_context>(http_context::shared_from_this()),
                _1, _2, _3,
                request,
                connection,
                bytes_to_read));
    } else
        handle_request(request, connection);
}

void http_context::read_body_handler(http_server::connection::input_range range, boost::system::error_code error, size_t bytes_transferred, http_server::request &request, http_server::connection_ptr connection, size_t bytes_to_read)
{
    if (!error)
    {
        std::exception_ptr ex_ptr;
        try {
            request.body.append(boost::begin(range), bytes_transferred);
            bytes_to_read -= bytes_transferred;
            read_body(request, connection, bytes_to_read);
        } catch(...) {
            ex_ptr = std::current_exception();
        }

        if (ex_ptr)
            handle_exception(connection, ex_ptr);
    } else {
        // TODO: Error handling
    }
}

void http_context::read_chunked_body(const http_server::request &request, http_server::connection_ptr connection, http_body_parser parser)
{
    if (parser.result() == http_body_parser::good) {
        http_server::request request_with_body = request;
        request_with_body.body = parser.data();
        handle_request(request_with_body, connection);
    } else {
        connection->read(
            boost::bind(
                &http_context::read_chunked_body_handler,
                std::static_pointer_cast<http_context>(http_context::shared_from_this()),
                _1, _2, _3,
                request,
                connection,
                parser));
    }
}

void http_context::read_chunked_body_handler(http_server::connection::input_range range, boost::system::error_code error, size_t bytes_transferred, http_server::request &request, http_server::connection_ptr connection, http_body_parser &parser)
{
    if (!error) {
        std::exception_ptr ex_ptr;
        try {
            parser.parse(boost::begin(range), boost::begin(range) + bytes_transferred);
            read_chunked_body(request, connection, parser);
        } catch (...) {
            ex_ptr = std::current_exception();
        }

        if (ex_ptr)
            handle_exception(connection, ex_ptr);
    } else {
        // TODO: Error handling
    }
}

void http_context::handle_request(const http_server::request &request, http_server::connection_ptr connection)
{
    remote_endpoint_ = boost::lexical_cast<std::string>(connection->socket().remote_endpoint());

    try {
        if (request.method == std::string("GET")) {
            handle_get_request(request, connection);
        } else if (request.method == std::string("PUT")) {
            handle_put_request(request, connection);
        } else
            throw parse_error();
    } catch (...) {
        remote_endpoint_.clear();
        throw;
    }

    remote_endpoint_.clear();
}

void http_context::handle_get_request(const http_server::request &request, http_server::connection_ptr connection)
{
    response response;
    response.status = response::ok;

    if (request.destination == std::string("/browse-xml")) {
        std::stringstream ss(response.content);
        {
            xml_visitor visitor(ss);
            browse(&visitor);
        }

        response.headers.push_back(http_server::response_header({ "Content-Type", "text/xml" }));
    } else if (request.destination == std::string("/browse-json")) {
        std::stringstream ss(response.content);
        {
            json_visitor visitor(ss);
            browse(&visitor);
        }

        response.headers.push_back(http_server::response_header({ "Content-Type", "application/json" }));
    } else {
        boost::any any_value = get_parameter(request.destination, '/');
        response.content = webservice_encoder::encode(any_value) + "\r\n";
        response.headers.push_back(http_server::response_header({ "Content-Type", (any_value.type() == typeid(decof::binary) ? "application/json" : "text/plain") }));
    }

    response.headers.push_back(http_server::response_header({ "Content-Length", std::to_string(response.content.length()) }));
    send_response(connection, response);
}

void http_context::handle_put_request(const http_server::request &request, http_server::connection_ptr connection)
{
    boost::any any_value;

    std::string body = request.body;
    boost::algorithm::trim_if(body, boost::is_any_of(" \f\n\r\t\v()"));

    std::string contentType = get_header(request.headers, "Content-Type");
    if (contentType == "vnd/com.toptica.decof.type.boolean") {
        if (body == "true")
            any_value = true;
        else if (body == "false")
            any_value = false;
        else
            throw invalid_value_error();
    } else if (contentType == "vnd/com.toptica.decof.type.integer") {
        throw std::runtime_error("Not yet implemented");
    } else if (contentType == "vnd/com.toptica.decof.type.real") {
        throw std::runtime_error("Not yet implemented");
    } else if (contentType == "vnd/com.toptica.decof.type.string") {
        throw std::runtime_error("Not yet implemented");
    } else if (contentType == "vnd/com.toptica.decof.type.binary") {
        throw std::runtime_error("Not yet implemented");
    } else
        throw wrong_type_error();

    set_parameter(request.destination, any_value, '/');
    send_response(connection, response::stock_reply(response::ok));
}

void http_context::handle_exception(http_server::connection_ptr connection, std::exception_ptr ex_ptr)
{
    if (!ex_ptr)
        return;

    response resp;
    try {
        std::rethrow_exception(ex_ptr);
    } catch (access_denied_error&) {
        resp = response::stock_reply(response::unauthorized);
    } catch (invalid_value_error&) {
        resp = response::stock_reply(response::bad_request);
    } catch (parse_error&) {
        resp = response::stock_reply(response::bad_request);
    } catch (...) {
        resp = response::stock_reply(response::internal_server_error);
    }

    send_response(connection, resp);
}

void http_context::send_response(http_server::connection_ptr connection, const http_context::response &response)
{
    connection->set_status(static_cast<http_server::connection::status_t>(response.status));
    connection->set_headers(response.headers);
    connection->write(response.content);
}

} // namespace decof
