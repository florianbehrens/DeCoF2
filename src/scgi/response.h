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

#ifndef DECOF_SCGI_RESPONSE_H
#define DECOF_SCGI_RESPONSE_H

#include <map>
#include <ostream>
#include <string>

namespace decof
{

namespace scgi
{

struct response
{
    enum class status_code {
      continue_ = 100,
      switching_protocols = 101,
      ok = 200,
      created = 201,
      accepted = 202,
      no_content = 204,
      partial_content = 206,
      multiple_choices = 300,
      moved_permanently = 301,
      moved_temporarily = 302,
      not_modified = 304,
      bad_request = 400,
      unauthorized = 401,
      forbidden = 403,
      not_found = 404,
      not_supported = 405,
      not_acceptable = 406,
      request_timeout = 408,
      precondition_failed = 412,
      unsatisfiable_range = 416,
      internal_server_error = 500,
      not_implemented = 501,
      bad_gateway = 502,
      service_unavailable = 503,
      space_unavailable = 507
    };

    std::string status_text() const;
    static response stock_response(status_code status);

    status_code status;
    std::map<std::string, std::string> headers;
    std::string body;
};

} // namespace scgi

} // namespace decof

/// Non-member stream insertion operator.
template<class Traits>
std::basic_ostream<char, Traits>& operator<<(std::basic_ostream<char, Traits> &os, const decof::scgi::response &resp)
{
    using decof::scgi::response;
    std::string str = resp.status_text();
    os << "HTTP/1.1 " << static_cast<size_t>(resp.status) << " " << resp.status_text() << "\r\n";
    for (const auto &header : resp.headers)
        os << header.first << ": " << header.second << "\r\n";
    if (resp.headers.count("Content-Type") == 0)
        os << "Content-Type: text/plain\r\n";

    if (!((resp.status >= response::status_code::continue_ && resp.status < response::status_code::ok) ||
           resp.status == response::status_code::no_content ||
           resp.status == response::status_code::not_modified)) {
        // No body allowed in other stati
        // (see http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.4)
        os << "Content-Length: " << resp.body.size() << "\r\n\r\n"
           << resp.body;
    } else
        os << "\r\n";

    os << std::flush;
    return os;
}

#endif // DECOF_SCGI_RESPONSE_H
