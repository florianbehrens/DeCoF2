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

#ifndef DECOF_SCGI_CONTEXT_H
#define DECOF_SCGI_CONTEXT_H

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/asio.hpp>

#include "client_context.h"
#include "request_parser.h"
#include "response.h"

namespace decof
{

namespace scgi
{

class scgi_context final : public client_context
{
public:
    scgi_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection, userlevel_t userlevel = Normal);

    virtual std::string connection_type() const override final;
    virtual std::string remote_endpoint() const override final;
    virtual void preload() override final;

private:
    /// SCGI request handler.
    void read_handler(const std::string &cstr);

    /// Handle HTTP GET request.
    void handle_get_request();

    /// Handle HTTP PUT request.
    void handle_put_request();

    /// Handles exceptions resulting from a HTTP request.
    /// @note It is required to do this in a function on its own due to the
    /// asynchronous request processing.
//    void handle_exception(http_server::connection_ptr connection, std::exception_ptr ex_ptr);

    /// Sends the given reply to the client.
    void send_response(const response &resp);

    /// The remote endpoint (HTTP client) as taken from the SCGI request.
    std::string remote_endpoint_;

    request_parser parser_;
};

} // namespace decof

} // namespace scgi

#endif // DECOF_SCGI_CONTEXT_H
