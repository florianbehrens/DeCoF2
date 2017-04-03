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

#include <exception>
#include <sstream>

#include <boost/any.hpp>

#include <websocket/response.h>

enum test_errors {
    test_error = -32700
};

namespace std {

template<>
struct is_error_code_enum<test_errors>
{
    static const bool value = true;
};

}

struct test_category : public std::error_category
{
    virtual const char* name() const noexcept override {
        return "test_error_category";
    }

    virtual std::string message(int condition) const override {
        return "Parse error";
    }
} the_test_category;

std::error_code make_error_code(test_errors e)
{
    return std::error_code(static_cast<int>(e), the_test_category);
}

BOOST_AUTO_TEST_SUITE(websocket_response)

BOOST_AUTO_TEST_CASE(response_with_value)
{
    const char nominal[] = "{\"jsonrpc\":\"2.0\",\"result\":true,\"id\":123}";
    decof::websocket::response r{ int64_t(123), boost::any(true) };

    std::stringstream out;
    out << r;

    BOOST_REQUIRE_EQUAL(out.str(), nominal);
}

BOOST_AUTO_TEST_CASE(response_without_value)
{
    const char nominal[] = "{\"jsonrpc\":\"2.0\",\"result\":null,\"id\":123}";
    decof::websocket::response r{ int64_t(123) };

    std::stringstream out;
    out << r;

    BOOST_REQUIRE_EQUAL(out.str(), nominal);
}

BOOST_AUTO_TEST_CASE(errorneous_response)
{
    const char nominal[] = "{\"jsonrpc\":\"2.0\",\"error\":{\"code\":-32700,\"message\":\"Parse error\"},\"id\":123}";
    decof::websocket::response r{ int64_t(123), boost::any(), std::error_code(test_error) };

    std::stringstream out;
    out << r;

    BOOST_REQUIRE_EQUAL(out.str(), nominal);
}

BOOST_AUTO_TEST_SUITE_END()
