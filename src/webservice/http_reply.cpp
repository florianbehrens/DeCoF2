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

#include "http_reply.h"

#include <map>
#include <string>

namespace
{

const char name_value_separator[] = { ':', ' ', '\0' };
const char crlf[] = { '\r', '\n', '\0' };

namespace status_strings
{

const std::string ok =
  "HTTP/1.0 200 OK\r\n";
const std::string bad_request =
  "HTTP/1.0 400 Bad Request\r\n";
const std::string unauthorized =
  "HTTP/1.0 401 Unauthorized\r\n";
const std::string not_found =
  "HTTP/1.0 404 Not Found\r\n";
const std::string unprocessable_entity =
  "HTTP/1.0 422 Unprocessable Entity\r\n";
const std::string internal_server_error =
  "HTTP/1.0 500 Internal Server Error\r\n";

const std::string& to_string(decof::http_reply::status_t status)
{
    switch (status)
    {
    case decof::http_reply::ok:
        return ok;
    case decof::http_reply::bad_request:
        return bad_request;
    case decof::http_reply::unauthorized:
        return unauthorized;
    case decof::http_reply::not_found:
        return not_found;
    case decof::http_reply::unprocessable_entity:
        return unprocessable_entity;
    default:
        return internal_server_error;
    }
}

} // namespace status_strings

namespace stock_replies
{

const std::string ok = "";
const std::string bad_request =
  "<html>"
  "<head><title>Bad Request</title></head>"
  "<body><h1>400 Bad Request</h1></body>"
  "</html>";
const std::string not_found =
  "<html>"
  "<head><title>Not Found</title></head>"
  "<body><h1>404 Not Found</h1></body>"
  "</html>";
const std::string unauthorized =
  "<html>"
  "<head><title>Unauthorized</title></head>"
  "<body><h1>401 Unauthorized</h1></body>"
  "</html>";
const std::string internal_server_error =
  "<html>"
  "<head><title>Internal Server Error</title></head>"
  "<body><h1>500 Internal Server Error</h1></body>"
  "</html>";

std::string to_string(decof::http_reply::status_t status)
{
    switch (status)
    {
    case decof::http_reply::ok:
        return ok;
    case decof::http_reply::bad_request:
        return bad_request;
    case decof::http_reply::unauthorized:
        return unauthorized;
    case decof::http_reply::not_found:
        return not_found;
    default:
        return internal_server_error;
    }
}

} // namespace stock_replies

} // Anonymous namespace

namespace decof
{

http_reply::http_reply(decof::http_reply::status_t status) :
    status_(status)
{}

void http_reply::header(const std::string &name, const std::string &value)
{
    headers_[name] = value;
}

std::string http_reply::header(const std::string &name) const
{
    std::string retval;

    try {
        retval = headers_.at(name);
    } catch (std::out_of_range&) {}

    return retval;
}

void http_reply::content(const std::string &content, const std::string &content_type)
{
    header("Content-Length", std::to_string(content.size()));
    header("Content-Type", content_type);
    content_ = content;
}

std::string http_reply::content() const
{
    return content_;
}

std::vector<boost::asio::const_buffer> http_reply::to_buffers()
{
    std::vector<boost::asio::const_buffer> buffers;
    buffers.push_back(boost::asio::buffer(status_strings::to_string(status_)));
    for (auto &h : headers_)
    {
        buffers.push_back(boost::asio::buffer(h.first));
        buffers.push_back(boost::asio::buffer(name_value_separator));
        buffers.push_back(boost::asio::buffer(h.second));
        buffers.push_back(boost::asio::buffer(crlf));
    }
    buffers.push_back(boost::asio::buffer(crlf));
    buffers.push_back(boost::asio::buffer(content_));
    return buffers;
}

std::string decof::http_reply::to_string() const
{
    std::string retval;
    retval += status_strings::to_string(status_);
    for (auto &h : headers_)
        retval += h.first + name_value_separator + h.second + crlf;

    retval += crlf + content_;
    return retval;
}

http_reply http_reply::stock_reply(http_reply::status_t status)
{
  http_reply r(status);
  r.content(stock_replies::to_string(status), "text/html");
  return r;
}

} // namespace decof
