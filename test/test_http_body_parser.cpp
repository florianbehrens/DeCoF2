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

#include <tuple>

#include <boost/test/unit_test.hpp>

#include "http_body_parser.h"

BOOST_AUTO_TEST_SUITE(http_body_parser)

struct fixture
{
    const std::string request_body = "0\r\n\r\n";
    decof::http_body_parser p;
};

BOOST_FIXTURE_TEST_CASE(multibyte_chunks, fixture)
{
    const std::string request_body = "6\r\nHallo \r\n6\r\nWelt!\n\r\n0\r\n\r\n";
    p.parse(request_body.cbegin(), request_body.cend());

    BOOST_REQUIRE(p.result() == decof::http_body_parser::good);
    BOOST_REQUIRE_EQUAL(p.data(), "Hallo Welt!\n");
}

BOOST_FIXTURE_TEST_CASE(single_byte_chunks, fixture)
{
    const std::string request_body = "1\r\nH\r\n1\r\na\r\n1\r\nl\r\n1\r\nl\r\n1\r\no\r\n1\r\n\n\r\n0\r\n\r\n";
    p.parse(request_body.cbegin(), request_body.cend());

    BOOST_REQUIRE(p.result() == decof::http_body_parser::good);
    BOOST_REQUIRE_EQUAL(p.data(), "Hallo\n");
}

BOOST_FIXTURE_TEST_CASE(empty_chunked_body, fixture)
{
    const std::string request_body = "0\r\n\r\n";
    p.parse(request_body.cbegin(), request_body.cend());

    BOOST_REQUIRE(p.result() == decof::http_body_parser::good);
    BOOST_REQUIRE(p.data().empty());
}

BOOST_AUTO_TEST_SUITE_END()
