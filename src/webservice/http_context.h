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

#ifndef DECOF_HTTP_CONTEXT_H
#define DECOF_HTTP_CONTEXT_H

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/network/include/http/server.hpp>

#include "client_context.h"
#include "http_body_parser.h"

namespace decof
{

class http_context : public client_context
{
    typedef boost::network::http::async_server<http_context> http_server;
    typedef boost::network::http::basic_response<boost::network::http::tags::http_server> response;
    typedef std::vector<boost::network::http::request_header_narrow> header_list;

public:
    http_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection, userlevel_t userlevel = Normal);

    virtual std::string connection_type() const override final;
    virtual std::string remote_endpoint() const override final;
    virtual void preload() override final;

    /// @brief Handle initial HTTP request.
    /// This callback is called by cpp netlib as soon as the HTTP header has
    /// arrived. There is yet no body.
    void operator()(http_server::request const & request, http_server::connection_ptr connection);

private:
    /// Read on in case of known length HTTP body.
    void read_body(const http_server::request &request, http_server::connection_ptr connection, size_t bytes_to_read);
    void read_body_handler(http_server::connection::input_range range, boost::system::error_code error, size_t bytes_transferred, http_server::request &request, http_server::connection_ptr connection, size_t bytes_to_read);

    /// Read on in case of chunked transfer encoding.
    /// @param bytes_to_read Remaining bytes of the current chunk or 0 for a
    /// new chunk.
    void read_chunked_body(const http_server::request &request, http_server::connection_ptr connection, http_body_parser parser = http_body_parser());
    void read_chunked_body_handler(http_server::connection::input_range range, boost::system::error_code error, size_t bytes_transferred, http_server::request &request, http_server::connection_ptr connection, http_body_parser &parser);

    /// Handle HTTP request including HTTP body.
    void handle_request(const http_server::request &request, http_server::connection_ptr connection);

    /// Handle HTTP GET request.
    void handle_get_request(const http_server::request &request, http_server::connection_ptr connection);

    /// Handle HTTP PUT request.
    void handle_put_request(const http_server::request &request, http_server::connection_ptr connection);

    /// Handles exceptions resulting from a HTTP request.
    /// @note It is required to do this in a function on its own due to the
    /// asynchronous request processing.
    void handle_exception(http_server::connection_ptr connection, std::exception_ptr ex_ptr);

    /// Sends the given response to the client.
    void send_response(http_server::connection_ptr connection, response response);

    // We must hold a shared pointer to the io_service here because the
    // http_server_ object does only hold a boost::shared_ptr with a custom,
    // empty deleter.
    std::shared_ptr<boost::asio::io_service> io_service_ptr_;

    http_server::options options_;
    std::unique_ptr<http_server> http_server_ptr_;

    std::string remote_endpoint_;
};

} // namespace decof

#endif // DECOF_HTTP_CONTEXT_H
