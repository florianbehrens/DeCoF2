/*
 * Copyright (c) 2017 Florian Behrens
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

#include <boost/any.hpp>

#include <decof/types.h>

#include <websocket/request.h>

BOOST_AUTO_TEST_SUITE(websocket_request)

BOOST_AUTO_TEST_CASE(boolean_set_request)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [
        "path.to.parameter.object",
        true
    ],
    "id": 1
}
)lit";

    decof::websocket::request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.path, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::boolean>(r.value), true);
}

BOOST_AUTO_TEST_CASE(boolean_sequence_set_request)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [
        "path.to.parameter.object",
        [ true, false, true, false ]
    ],
    "id": 1
}
)lit";

    decof::websocket::request r;
    r.parse(input, input + sizeof(input));

    std::vector<boost::any> nominal{ boost::any(true), boost::any(false), boost::any(true), boost::any(false) };
    auto actual = boost::any_cast<std::vector<boost::any>>(r.value);

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.path, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(actual.size(), nominal.size());

    for (size_t i = 0; i < nominal.size(); ++i)
        BOOST_REQUIRE_EQUAL(boost::any_cast<bool>(nominal[i]), boost::any_cast<bool>(actual[i]));
}

BOOST_AUTO_TEST_CASE(integer_set_request_min)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [
        "path.to.parameter.object",
        -9223372036854775808
    ],
    "id": 1
}
)lit";

    decof::websocket::request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.path, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::integer>(r.value), std::numeric_limits<int64_t>::min());
}

BOOST_AUTO_TEST_CASE(integer_set_request_max)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [
        "path.to.parameter.object",
        9223372036854775807
    ],
    "id": 1
}
)lit";

    decof::websocket::request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.path, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::integer>(r.value), std::numeric_limits<int64_t>::max());
}

BOOST_AUTO_TEST_CASE(real_set_request)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [
        "path.to.parameter.object",
        123.456
    ],
    "id": 1
}
)lit";

    decof::websocket::request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.path, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::real>(r.value), 123.456);
}

BOOST_AUTO_TEST_CASE(string_set_request)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [
        "path.to.parameter.object",
        "\"string\" value"
    ],
    "id": 1
}
)lit";

    decof::websocket::request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.path, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::string>(r.value), "\"string\" value");
}

BOOST_AUTO_TEST_CASE(invalid_jsonrpc_version)
{
    const char input[] = R"lit({
    "jsonrpc": "1.0",
    "method": "get",
    "params": [ "path.to.parameter.object" ],
    "id": 1
}
)lit";

    decof::websocket::request r;

    BOOST_REQUIRE_THROW(r.parse(input, input + sizeof(input)), decof::parse_error);
}

BOOST_AUTO_TEST_SUITE_END()
