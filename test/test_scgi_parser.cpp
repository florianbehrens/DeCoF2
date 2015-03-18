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

#include <boost/test/unit_test.hpp>

#include "scgi/request_parser.h"

BOOST_AUTO_TEST_SUITE(scgi_parser)

struct fixture
{
    fixture() {
        parser.reset();
    }

    decof::scgi::request_parser parser;
};

BOOST_FIXTURE_TEST_CASE(empty_request, fixture)
{
    const char *input = "0:,";
    auto result = parser.parse(input, input + ::strlen(input));
    BOOST_REQUIRE_EQUAL(result, decof::scgi::request_parser::good);
    BOOST_REQUIRE_EQUAL(parser.body, "");
}

BOOST_FIXTURE_TEST_CASE(minimal_request, fixture)
{
    const char input[] = "24:CONTENT_LENGTH\0000\0SCGI\0001\0,";
    auto result = parser.parse(input, input + sizeof(input) - 1);
    BOOST_REQUIRE_EQUAL(result, decof::scgi::request_parser::good);
    BOOST_REQUIRE_EQUAL(parser.headers.at("CONTENT_LENGTH"), std::string("0"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SCGI"), std::string("1"));
    BOOST_REQUIRE_EQUAL(parser.body, "");
}

BOOST_FIXTURE_TEST_CASE(typical_request, fixture)
{
    const char input[] =
        "584:"
        "CONTENT_LENGTH\0"          "0\0"
        "SCGI\0"                    "1\0"
        "SERVER_SOFTWARE\0"         "lighttpd/1.4.33\0"
        "SERVER_NAME\0"             "localhost\0"
        "GATEWAY_INTERFACE\0"       "CGI/1.1\0"
        "SERVER_PORT\0"             "80\0"
        "SERVER_ADDR\0"             "127.0.0.1\0"
        "REMOTE_PORT\0"             "39997\0"
        "REMOTE_ADDR\0"             "127.0.0.1\0"
        "SCRIPT_NAME\0"             "/example\0"
        "PATH_INFO\0"               "/subnode/boolean\0"
        "PATH_TRANSLATED\0"         "/var/www/subnode/boolean\0"
        "SCRIPT_FILENAME\0"         "/var/www/example\0"
        "DOCUMENT_ROOT\0"           "/var/www\0"
        "REQUEST_URI\0"             "/example/subnode/boolean\0"
        "QUERY_STRING\0\0"
        "REQUEST_METHOD\0"          "PUT\0"
        "REDIRECT_STATUS\0"         "200\0"
        "SERVER_PROTOCOL\0"         "HTTP/1.1\0"
        "HTTP_USER_AGENT\0"         "curl/7.37.1\0"
        "HTTP_HOST\0"               "localhost\0"
        "HTTP_ACCEPT\0"             "*/*\0"
        "HTTP_TRANSFER_ENCODING\0"  "chunked\0"
        "CONTENT_TYPE\0"            "vnd/com.toptica.decof.type.boolean\0"
        ",";

    auto result = parser.parse(input, input + sizeof(input) - 1);

    BOOST_REQUIRE_EQUAL(result, decof::scgi::request_parser::good);
    BOOST_REQUIRE_EQUAL(parser.method, decof::scgi::request_parser::method_type::put);
    BOOST_REQUIRE_EQUAL(parser.uri,    std::string("/example/subnode/boolean"));

    BOOST_REQUIRE_EQUAL(parser.headers.at("CONTENT_LENGTH"),          std::string("0"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SCGI"),                    std::string("1"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SERVER_SOFTWARE"),         std::string("lighttpd/1.4.33"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SERVER_NAME"),             std::string("localhost"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("GATEWAY_INTERFACE"),       std::string("CGI/1.1"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SERVER_PORT"),             std::string("80"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SERVER_ADDR"),             std::string("127.0.0.1"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("REMOTE_PORT"),             std::string("39997"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("REMOTE_ADDR"),             std::string("127.0.0.1"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SCRIPT_NAME"),             std::string("/example"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("PATH_INFO"),               std::string("/subnode/boolean"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("PATH_TRANSLATED"),         std::string("/var/www/subnode/boolean"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SCRIPT_FILENAME"),         std::string("/var/www/example"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("DOCUMENT_ROOT"),           std::string("/var/www"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("REQUEST_URI"),             std::string("/example/subnode/boolean"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("QUERY_STRING"),            std::string());
    BOOST_REQUIRE_EQUAL(parser.headers.at("REQUEST_METHOD"),          std::string("PUT"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("REDIRECT_STATUS"),         std::string("200"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SERVER_PROTOCOL"),         std::string("HTTP/1.1"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("HTTP_USER_AGENT"),         std::string("curl/7.37.1"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("HTTP_HOST"),               std::string("localhost"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("HTTP_ACCEPT"),             std::string("*/*"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("HTTP_TRANSFER_ENCODING"),  std::string("chunked"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("CONTENT_TYPE"),            std::string("vnd/com.toptica.decof.type.boolean"));
    BOOST_REQUIRE_EQUAL(parser.body, "");
}

BOOST_FIXTURE_TEST_CASE(request_with_body, fixture)
{
    const char input[] = "25:CONTENT_LENGTH\00010\0SCGI\0001\0,Hallo Welt";
    auto result = parser.parse(input, input + sizeof(input) - 1);
    BOOST_REQUIRE_EQUAL(result, decof::scgi::request_parser::good);
    BOOST_REQUIRE_EQUAL(parser.headers.at("CONTENT_LENGTH"), std::string("10"));
    BOOST_REQUIRE_EQUAL(parser.headers.at("SCGI"), std::string("1"));
    BOOST_REQUIRE_EQUAL(parser.body, "Hallo Welt");
}

BOOST_AUTO_TEST_SUITE_END()
