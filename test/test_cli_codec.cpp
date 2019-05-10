/*
 * Copyright (c) 2016 Florian Behrens
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

#include <cli/decoder.h>
#include <boost/test/unit_test.hpp>
#include <string>

BOOST_AUTO_TEST_SUITE(cli_codec)

BOOST_AUTO_TEST_CASE(standard_escape_sequences)
{
    std::string str("\\a\\b\\f\\n\\r\\t\\v\\\\\\'\\\"\\?");
    str.resize(decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()));

    BOOST_REQUIRE_EQUAL(str, "\x07\x08\x0C\x0A\x0D\x09\x0B\x5C\x27\x22\x3F");
}

BOOST_AUTO_TEST_CASE(hexadecimal_escape_sequences)
{
    std::string str("\\x00\\x9F\\xA0\\xFF");
    std::string exp{'\x00', '\x9F', '\xA0', '\xFF'};
    std::string res;

    decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), std::back_insert_iterator<std::string>(res));

    BOOST_REQUIRE_EQUAL(exp, res);
}

BOOST_AUTO_TEST_CASE(invalid_character1)
{
    std::string str("abc\x1F");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_CASE(invalid_character2)
{
    std::string str("abc\n");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_CASE(invalid_character3)
{
    std::string str("abc\x80");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_CASE(invalid_character4)
{
    std::string str("abc\x80");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_CASE(invalid_escape_sequence)
{
    // Test invalid escape sequence '\x'
    std::string str("abc\\x");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_CASE(invalid_hex_escape_sequence1)
{
    std::string str("abc\\xG0");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_CASE(invalid_hex_escape_sequence2)
{
    std::string str("abc\\x0G");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_CASE(backslash_as_last_char)
{
    std::string str("abc\\");
    BOOST_REQUIRE_THROW(
        decof::cli::backslash_escape_decoder(str.cbegin(), str.cend(), str.begin()), decof::parse_error);
}

BOOST_AUTO_TEST_SUITE_END()
