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

#include <iostream>
#include <thread>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <decof/cli/update_container.h>
#include <decof/types.h>

BOOST_AUTO_TEST_SUITE(cli_update_container)

using namespace decof;

struct fixture
{
    cli::update_container updates_;
};

BOOST_FIXTURE_TEST_CASE(initial_empty, fixture)
{
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
}

BOOST_FIXTURE_TEST_CASE(push_single_element, fixture)
{
    const std::string nominal_uri("root");
    const value_t nominal_value(integer_t(0));

    cli::update_container::time_point before = std::chrono::system_clock::now();
    updates_.push(nominal_uri, nominal_value);
    cli::update_container::time_point after = std::chrono::system_clock::now();
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    std::string actual_uri;
    cli::update_container::time_point time;
    value_t actual_value;

    std::tie(actual_uri, actual_value, time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);

    BOOST_REQUIRE_EQUAL(actual_uri, nominal_uri);
    BOOST_REQUIRE(actual_value == nominal_value);
    BOOST_REQUIRE(before <= time);
    BOOST_REQUIRE(time <= after);
}

BOOST_FIXTURE_TEST_CASE(push_equal_elements, fixture)
{
    const std::string nominal_uri("very:very:very:lengthy:parameter:path");
    const value_t value(string_t(10, 'A'));

    // Push a huge number of equal key updates
    const size_t count = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
        updates_.push(nominal_uri, value);
    }
    auto duration = std::chrono::high_resolution_clock::now() - start;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Check current time
    cli::update_container::time_point before = std::chrono::system_clock::now();
    const value_t nominal_value(string_t(10, 'X'));
    updates_.push(nominal_uri, nominal_value);
    cli::update_container::time_point after = std::chrono::system_clock::now();
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    std::string actual_uri;
    value_t actual_value;
    cli::update_container::time_point time;

    std::tie(actual_uri, actual_value, time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);

    BOOST_REQUIRE_EQUAL(actual_uri, nominal_uri);
    BOOST_REQUIRE(actual_value == nominal_value);
    BOOST_REQUIRE(before <= time);
    BOOST_REQUIRE(time <= after);

    std::cout << "Pushing " << count << " equal elements into container took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
              << " ms (~ "
              << std::chrono::duration_cast<std::chrono::microseconds>(duration/count).count()
              << " µs per element)" << std::endl;
}

BOOST_FIXTURE_TEST_CASE(push_different_elements, fixture)
{
    std::string uri("very:very:very:lengthy:parameter:path:");

    // Push a reasonably big number of different key updates
    const size_t count = 1000000;
    cli::update_container::time_point before = std::chrono::system_clock::now();
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
        updates_.push(uri + std::to_string(i), integer_t(i));
    }
    auto duration = std::chrono::high_resolution_clock::now() - start;
    cli::update_container::time_point after = std::chrono::system_clock::now();

    std::cout << "Pushing " << count << " different elements into container took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
              << " ms (~ "
              << std::chrono::duration_cast<std::chrono::microseconds>(duration/count).count()
              << " µs per element)" << std::endl;

    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
        std::string actual_uri;
        cli::update_container::time_point time;
        value_t actual_value;

        std::tie(actual_uri, actual_value, time) = updates_.pop_front();
    }
    duration = std::chrono::high_resolution_clock::now() - start;

    std::cout << "Popping " << count << " different elements from container took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
              << " ms (~ "
              << std::chrono::duration_cast<std::chrono::microseconds>(duration/count).count()
              << " µs per element)" << std::endl;

    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
}

BOOST_FIXTURE_TEST_CASE(push_arbitrary_elements, fixture)
{
    cli::update_container::time_point time[6];

    time[0] = std::chrono::system_clock::now();
    updates_.push("parameter1", integer_t(1));
    time[1] = std::chrono::system_clock::now();
    updates_.push("parameter2", string_t("value"));
    time[2] = std::chrono::system_clock::now();
    updates_.push("parameter1", integer_t(2));
    time[3] = std::chrono::system_clock::now();
    updates_.push("parameter3", real_t(1.0));
    time[4] = std::chrono::system_clock::now();
    updates_.push("parameter1", integer_t(3));
    time[5] = std::chrono::system_clock::now();

    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    std::string actual_uri;
    cli::update_container::time_point actual_time;
    value_t actual_value;

    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL("parameter1", actual_uri);
    BOOST_REQUIRE(value_t{ integer_t(3) } == actual_value);
    BOOST_REQUIRE(time[4] <= actual_time && actual_time <= time[5]);

    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL("parameter2", actual_uri);
    BOOST_REQUIRE(value_t{ string_t("value") } == actual_value);
    BOOST_REQUIRE(time[1] <= actual_time && actual_time <= time[2]);

    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL("parameter3", actual_uri);
    BOOST_REQUIRE(value_t{ 1.0 } == actual_value);
    BOOST_REQUIRE(time[3] <= actual_time && actual_time <= time[4]);
}

BOOST_FIXTURE_TEST_CASE(empty_and_push_element, fixture)
{
    std::string actual_uri;
    cli::update_container::time_point actual_time;
    value_t actual_value;

    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
    updates_.push("parameter", integer_t(1));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);
    updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
    updates_.push("parameter", integer_t(2));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);
    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);

    BOOST_REQUIRE_EQUAL("parameter", actual_uri);
    BOOST_REQUIRE(value_t{ integer_t(2) } == actual_value);
}

BOOST_FIXTURE_TEST_CASE(push_and_pop_interchangeably, fixture)
{
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
    updates_.push("c", integer_t(1));
    updates_.push("b", integer_t(2));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    updates_.push("a", integer_t(3));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    updates_.pop_front();
    updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
}

BOOST_AUTO_TEST_SUITE_END()
