/*
 * Copyright (c) 2014 Florian Behrens
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

#include "webservice.h"

#include "object_dictionary.h"
#include "http_reply.h"
#include "json_visitor.h"
#include "request.hpp"
#include "tcp_connection.h"
#include "xml_visitor.h"

namespace decof
{

void decof::webservice::preload()
{
    // Connect to signals of connection class
    connection_->read_signal.connect(std::bind(&webservice::read_handler, this, std::placeholders::_1));
    connection_->write_signal.connect(std::bind(&webservice::write_handler, this));
    connection_->disconnect_signal.connect(std::bind(&webservice::disconnect_handler, this));

    connection_->async_read_some();
}

void webservice::read_handler(const std::string &cstr)
{
    http::server::request request;
    http::server::request_parser::result_type result;

    std::tie(result, std::ignore) = request_parser_.parse(request, cstr.cbegin(), cstr.cend());

    if (result == http::server::request_parser::indeterminate)
        connection_->async_read_some();
    else if (result == http::server::request_parser::good) {
        http_reply reply;

        if (request.method == std::string("GET")) {
            if (request.uri == std::string("/browse-xml")) {
                std::stringstream ss;
                {
                    xml_visitor visitor(ss);
                    browse(&visitor);
                }
                reply.content(ss.str(), "text/xml");
            } else if (request.uri == std::string("/browse-json")) {
                std::stringstream ss;
                {
                    json_visitor visitor(ss);
                    browse(&visitor);
                }
                reply.content(ss.str(), "application/json");
            } else
                reply = http_reply::stock_reply(http_reply::not_found);
        }

        connection_->async_write(reply.to_string());
    }
    else if (result == http::server::request_parser::bad)
    {
        http_reply reply = http_reply::stock_reply(http_reply::bad_request);
        connection_->async_write(reply.to_string());
    }
    else
        connection_->async_read_some();
}

void webservice::write_handler()
{
    connection_->disconnect();
}

void webservice::disconnect_handler()
{
    // Remove this client context from object dictionary. Because it is a
    // shared pointer, it gets deleted after leaving function scope.
    auto self = shared_from_this();
    object_dictionary_.remove_context(self);
}

} // namespace decof
