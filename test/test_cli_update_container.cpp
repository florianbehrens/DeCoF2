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

BOOST_AUTO_TEST_SUITE(cli_update_container)

struct fixture
{
    decof::cli::update_container updates_;
};

BOOST_FIXTURE_TEST_CASE(initial_empty, fixture)
{
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
}

BOOST_FIXTURE_TEST_CASE(push_single_element, fixture)
{
    const std::string nominal_uri("root");
    const boost::any nominal_value(0);

    decof::cli::update_container::time_point before = std::chrono::system_clock::now();
    updates_.push(nominal_uri, nominal_value);
    decof::cli::update_container::time_point after = std::chrono::system_clock::now();
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    std::string actual_uri;
    decof::cli::update_container::time_point time;
    boost::any actual_value;

    std::tie(actual_uri, actual_value, time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);

    BOOST_REQUIRE_EQUAL(actual_uri, nominal_uri);
    BOOST_REQUIRE_EQUAL(boost::any_cast<int>(actual_value), boost::any_cast<int>(nominal_value));
    BOOST_REQUIRE(before <= time);
    BOOST_REQUIRE(time <= after);
}

BOOST_FIXTURE_TEST_CASE(push_equal_elements, fixture)
{
    const std::string nominal_uri("very:very:very:lengthy:parameter:path");
    const boost::any value(std::string(10, 'A'));

    // Push a huge number of equal key updates
    const size_t count = 1000000;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
        updates_.push(nominal_uri, value);
    }
    auto duration = std::chrono::high_resolution_clock::now() - start;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Check current time
    decof::cli::update_container::time_point before = std::chrono::system_clock::now();
    const boost::any nominal_value(std::string(10, 'X'));
    updates_.push(nominal_uri, nominal_value);
    decof::cli::update_container::time_point after = std::chrono::system_clock::now();
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    std::string actual_uri;
    boost::any actual_value;
    decof::cli::update_container::time_point time;

    std::tie(actual_uri, actual_value, time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);

    BOOST_REQUIRE_EQUAL(actual_uri, nominal_uri);
    BOOST_REQUIRE_EQUAL(boost::any_cast<std::string>(actual_value), boost::any_cast<const std::string>(nominal_value));
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
    decof::cli::update_container::time_point before = std::chrono::system_clock::now();
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
        updates_.push(uri + std::to_string(i), boost::any(i));
    }
    auto duration = std::chrono::high_resolution_clock::now() - start;
    decof::cli::update_container::time_point after = std::chrono::system_clock::now();

    std::cout << "Pushing " << count << " different elements into container took "
              << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
              << " ms (~ "
              << std::chrono::duration_cast<std::chrono::microseconds>(duration/count).count()
              << " µs per element)" << std::endl;

    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
        std::string actual_uri;
        decof::cli::update_container::time_point time;
        boost::any actual_value;

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
    decof::cli::update_container::time_point time[6];

    time[0] = std::chrono::system_clock::now();
    updates_.push("parameter1", boost::any(1));
    time[1] = std::chrono::system_clock::now();
    updates_.push("parameter2", boost::any(std::string("value")));
    time[2] = std::chrono::system_clock::now();
    updates_.push("parameter1", boost::any(2));
    time[3] = std::chrono::system_clock::now();
    updates_.push("parameter3", boost::any(1.0));
    time[4] = std::chrono::system_clock::now();
    updates_.push("parameter1", boost::any(3));
    time[5] = std::chrono::system_clock::now();

    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    std::string actual_uri;
    decof::cli::update_container::time_point actual_time;
    boost::any actual_value;

    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL("parameter1", actual_uri);
    BOOST_REQUIRE_EQUAL(3, boost::any_cast<int>(actual_value));
    BOOST_REQUIRE(time[4] <= actual_time && actual_time <= time[5]);

    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL("parameter2", actual_uri);
    BOOST_REQUIRE_EQUAL(std::string("value"), boost::any_cast<std::string>(actual_value));
    BOOST_REQUIRE(time[1] <= actual_time && actual_time <= time[2]);

    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL("parameter3", actual_uri);
    BOOST_REQUIRE_EQUAL(1.0, boost::any_cast<double>(actual_value));
    BOOST_REQUIRE(time[3] <= actual_time && actual_time <= time[4]);
}

BOOST_FIXTURE_TEST_CASE(empty_and_push_element, fixture)
{
    std::string actual_uri;
    decof::cli::update_container::time_point actual_time;
    boost::any actual_value;

    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
    updates_.push("parameter", boost::any(1));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);
    updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
    updates_.push("parameter", boost::any(2));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);
    std::tie(actual_uri, actual_value, actual_time) = updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);

    BOOST_REQUIRE_EQUAL("parameter", actual_uri);
    BOOST_REQUIRE_EQUAL(2, boost::any_cast<int>(actual_value));
}

BOOST_FIXTURE_TEST_CASE(push_and_pop_interchangeably, fixture)
{
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
    updates_.push("c", boost::any(1));
    updates_.push("b", boost::any(2));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    updates_.push("a", boost::any(3));
    BOOST_REQUIRE_EQUAL(updates_.empty(), false);

    updates_.pop_front();
    updates_.pop_front();
    BOOST_REQUIRE_EQUAL(updates_.empty(), true);
}

BOOST_AUTO_TEST_SUITE_END()
