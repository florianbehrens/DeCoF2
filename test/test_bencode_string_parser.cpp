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

#include "scgi/bencode_string_parser.h"

using namespace decof;

BOOST_AUTO_TEST_SUITE(bencode_string_parser)

struct fixture
{
    scgi::bencode_string_parser parser;
};

BOOST_FIXTURE_TEST_CASE(empty_string, fixture)
{
    std::string str = "0:";
    auto retval = parser.parse(str.cbegin(), str.cend());
    BOOST_REQUIRE_EQUAL(std::get<0>(retval), scgi::bencode_string_parser::good);
    BOOST_REQUIRE(parser.data.empty());
}

BOOST_FIXTURE_TEST_CASE(normal_string, fixture)
{
    std::string str = "12:Hallo Welt\r\n";
    auto retval = parser.parse(str.cbegin(), str.cend());
    BOOST_REQUIRE_EQUAL(std::get<0>(retval), scgi::bencode_string_parser::good);
    BOOST_REQUIRE_EQUAL(parser.data, "Hallo Welt\r\n");
}

BOOST_FIXTURE_TEST_CASE(negative_string_length, fixture)
{
    std::string str = "-5:Hallo";
    auto retval = parser.parse(str.cbegin(), str.cend());
    BOOST_REQUIRE_EQUAL(std::get<0>(retval), scgi::bencode_string_parser::bad);
}

BOOST_FIXTURE_TEST_CASE(no_string_length, fixture)
{
    std::string str = ":";
    auto retval = parser.parse(str.cbegin(), str.cend());
    BOOST_REQUIRE_EQUAL(std::get<0>(retval), scgi::bencode_string_parser::bad);
}

BOOST_FIXTURE_TEST_CASE(wrong_string_length, fixture)
{
    std::string str = "11:Hallo Welt";
    auto retval = parser.parse(str.cbegin(), str.cend());
    BOOST_REQUIRE_EQUAL(std::get<0>(retval), scgi::bencode_string_parser::indeterminate);
}

BOOST_AUTO_TEST_SUITE_END()
