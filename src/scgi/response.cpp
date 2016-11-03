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

#include <decof/scgi/response.h>

#include <map>
#include <sstream>

namespace
{

using decof::scgi::response;

static std::map<response::status_code, const char*> http_response_texts = {
    { response::status_code::ok                   , "OK" },
    { response::status_code::created              , "Created" },
    { response::status_code::accepted             , "Accepted" },
    { response::status_code::no_content           , "No Content" },
    { response::status_code::partial_content      , "Partial Content" },
    { response::status_code::multiple_choices     , "Multiple Choices" },
    { response::status_code::moved_permanently    , "Moved Permanently" },
    { response::status_code::moved_temporarily    , "Moved Temporarily" },
    { response::status_code::not_modified         , "Not Modified" },
    { response::status_code::bad_request          , "Bad Request" },
    { response::status_code::unauthorized         , "Unauthorized" },
    { response::status_code::forbidden            , "Fobidden" },
    { response::status_code::not_found            , "Not Found" },
    { response::status_code::not_supported        , "Not Supported" },
    { response::status_code::not_acceptable       , "Not Acceptable" },
    { response::status_code::request_timeout      , "Request Timeout" },
    { response::status_code::precondition_failed  , "Precondition Failed" },
    { response::status_code::unsatisfiable_range  , "Requested Range Not Satisfiable" },
    { response::status_code::internal_server_error, "Internal Server Error" },
    { response::status_code::not_implemented      , "Not Implemented" },
    { response::status_code::bad_gateway          , "Bad Gateway" },
    { response::status_code::service_unavailable  , "Service Unavailable" },
    { response::status_code::space_unavailable    , "Insufficient Space to Store Resource" }
};

} // Anonymous namespace

namespace decof
{

namespace scgi
{

std::string response::status_text() const
{
    return http_response_texts.at(status);
}

response response::stock_response(response::status_code status)
{
    response resp;
    resp.status = status;
    return resp;
}

} // namespace scgi

} // namespace decof
