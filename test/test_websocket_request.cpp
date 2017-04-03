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

#include <websocket/error.h>
#include <websocket/request.h>

BOOST_AUTO_TEST_SUITE(websocket_request)

using decof::websocket::request;

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

    request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.uri, "path.to.parameter.object");
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

    request r;
    r.parse(input, input + sizeof(input));

    std::vector<boost::any> nominal{ boost::any(true), boost::any(false), boost::any(true), boost::any(false) };
    auto actual = boost::any_cast<std::vector<boost::any>>(r.value);

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.uri, "path.to.parameter.object");
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
        -2147483648
    ],
    "id": 1
}
)lit";

    request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.uri, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::integer>(r.value), std::numeric_limits<int32_t>::min());
}

BOOST_AUTO_TEST_CASE(integer_set_request_max)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [
        "path.to.parameter.object",
        2147483647
    ],
    "id": 1
}
)lit";

    request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.uri, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::integer>(r.value), std::numeric_limits<int32_t>::max());
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

    request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.uri, "path.to.parameter.object");
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

    request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.uri, "path.to.parameter.object");
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

    BOOST_REQUIRE_THROW(request().parse(input, input + sizeof(input)), decof::bad_request_error);
}

BOOST_AUTO_TEST_CASE(null_value_type)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [ "path.to.parameter.object", null ],
    "id": 1
}
)lit";

    BOOST_REQUIRE_THROW(request().parse(input, input + sizeof(input)), decof::websocket::invalid_params_error);
}

BOOST_AUTO_TEST_CASE(empty_params_array)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [],
    "id": 1
}
)lit";

    BOOST_REQUIRE_THROW(request().parse(input, input + sizeof(input)), decof::bad_request_error);
}

BOOST_AUTO_TEST_CASE(params_array_too_long)
{
    const char input[] = R"lit({
    "method": "set",
    "params": [ "path.to.parameter.object", 0, 1 ],
    "id": 1
}
)lit";

    BOOST_REQUIRE_THROW(request().parse(input, input + sizeof(input)), decof::websocket::invalid_params_error);
}

BOOST_AUTO_TEST_CASE(params_object)
{
    const char input[] = R"lit({
    "method": "set",
    "params": {
        "uri": "path.to.parameter.object",
        "value": true
    },
    "id": 1
}
)lit";

    request r;
    r.parse(input, input + sizeof(input));

    BOOST_REQUIRE_EQUAL(r.method, "set");
    BOOST_REQUIRE_EQUAL(r.uri, "path.to.parameter.object");
    BOOST_REQUIRE_EQUAL(boost::any_cast<decof::boolean>(r.value), true);
}

BOOST_AUTO_TEST_CASE(params_object_array)
{
    const char input[] = R"lit({
    "method": "set",
    "params": {
        "uri": "path.to.parameter.object",
        "value": [ true, false ]
    },
    "id": 1
}
)lit";

    request r;
    r.parse(input, input + sizeof(input));

    std::vector<boost::any> nominal{ boost::any(true), boost::any(false) };
    auto actual = boost::any_cast<std::vector<boost::any>>(r.value);

    BOOST_REQUIRE_EQUAL(actual.size(), nominal.size());

    for (auto i = 0; i < nominal.size(); ++i)
        BOOST_REQUIRE_EQUAL(boost::any_cast<bool>(nominal[i]), boost::any_cast<bool>(actual[i]));
}

BOOST_AUTO_TEST_SUITE_END()
