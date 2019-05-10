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

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <decof/scgi/response.h>

BOOST_AUTO_TEST_SUITE(scgi_response)

struct fixture
{
    decof::scgi::response resp;
};

BOOST_FIXTURE_TEST_CASE(empty_response_ok, fixture)
{
    std::stringstream ss;
    decof::scgi::response resp = decof::scgi::response::stock_response(decof::scgi::response::status_code::ok);
    ss << resp;

    std::string result =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 0\r\n\r\n";
    BOOST_REQUIRE_EQUAL(ss.str(), result);
}

BOOST_FIXTURE_TEST_CASE(empty_response_error, fixture)
{
    std::stringstream ss;
    decof::scgi::response resp = decof::scgi::response::stock_response(decof::scgi::response::status_code::internal_server_error);
    ss << resp;

    std::string result =
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 0\r\n\r\n";
    BOOST_REQUIRE_EQUAL(ss.str(), result);
}

BOOST_FIXTURE_TEST_CASE(response_with_header, fixture)
{
    std::stringstream ss;
    decof::scgi::response resp = decof::scgi::response::stock_response(decof::scgi::response::status_code::ok);
    resp.headers["Content-Type"] = "text/plain";
    ss << resp;

    std::string result =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 0\r\n\r\n";
    BOOST_REQUIRE_EQUAL(ss.str(), result);
}

BOOST_FIXTURE_TEST_CASE(response_with_body, fixture)
{
    std::stringstream ss;
    decof::scgi::response resp = decof::scgi::response::stock_response(decof::scgi::response::status_code::ok);
    resp.body = "Hi there!";
    ss << resp;

    std::string result =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 9\r\n\r\n"
        "Hi there!";
    BOOST_REQUIRE_EQUAL(ss.str(), result);
}

BOOST_FIXTURE_TEST_CASE(response_with_status_no_content, fixture)
{
    std::stringstream ss;
    decof::scgi::response resp = decof::scgi::response::stock_response(decof::scgi::response::status_code::no_content);
    ss << resp;

    std::string result =
        "HTTP/1.1 204 No Content\r\n"
        "Content-Type: text/plain\r\n\r\n";
    BOOST_REQUIRE_EQUAL(ss.str(), result);
}

BOOST_FIXTURE_TEST_CASE(response_with_status_not_modified, fixture)
{
    std::stringstream ss;
    decof::scgi::response resp = decof::scgi::response::stock_response(decof::scgi::response::status_code::not_modified);
    ss << resp;

    std::string result =
        "HTTP/1.1 304 Not Modified\r\n"
        "Content-Type: text/plain\r\n\r\n";
    BOOST_REQUIRE_EQUAL(ss.str(), result);
}

BOOST_AUTO_TEST_SUITE_END()
