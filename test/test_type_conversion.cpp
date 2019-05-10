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

#include <decof/types.h>
#include <decof/conversion.h>
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <numeric>
#include <vector>
#include "test_helpers.h"

using namespace decof;

BOOST_AUTO_TEST_SUITE(type_conversion)

using decof_types = boost::mpl::list<
    bool,
    short, unsigned short, int, unsigned int, long, unsigned long,
    float, double,
    std::string,
    std::array<int, 3>,
    std::vector<bool>,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(round_trip_conversion, T, decof_types)
{
    auto nominal = T();
    auto actual  = conversion_helper<T>::from_generic(conversion_helper<T>::to_generic(nominal));

    BOOST_REQUIRE(actual == nominal);
}

BOOST_AUTO_TEST_CASE(empty_value_t_throws)
{
    value_t empty;
    BOOST_REQUIRE_THROW(conversion_helper<int>::from_generic(empty), wrong_type_error);
}

BOOST_AUTO_TEST_CASE(integral_type_convertible_to_floating_point)
{
    const integer_t nominal = (1ll << std::numeric_limits<float>::digits) - 1;

    value_t val_min{ -nominal };
    BOOST_REQUIRE_EQUAL(conversion_helper<float>::from_generic(val_min), -nominal);

    value_t val_max{ nominal };
    BOOST_REQUIRE_EQUAL(conversion_helper<float>::from_generic(val_max), nominal);
}

BOOST_AUTO_TEST_CASE(floating_point_type_convertible_to_integral)
{
    const long long nominal = (1ll << std::numeric_limits<real_t>::digits) - 1;

    value_t val_min{ static_cast<real_t>(-nominal) };
    BOOST_REQUIRE_EQUAL(conversion_helper<long long>::from_generic(val_min), -nominal);

    value_t val_max{ static_cast<real_t>(nominal) };
    BOOST_REQUIRE_EQUAL(conversion_helper<long long>::from_generic(val_max), nominal);
}

BOOST_AUTO_TEST_CASE(conversion_from_string_to_string_t)
{
    std::string nominal = "Hello World";

    auto const gen = conversion_helper<std::string>::to_generic(nominal);
    BOOST_REQUIRE_NO_THROW(boost::get<string_t>(boost::get<scalar_t>(gen)));
}

BOOST_AUTO_TEST_CASE(conversion_from_const_char_ptr_to_string_t)
{
    const char* nominal = "Hello World";
    auto const actual = boost::get<string_t>(boost::get<scalar_t>(conversion_helper<const char*>::to_generic(nominal)));

    BOOST_REQUIRE_EQUAL(nominal, actual);
}

BOOST_AUTO_TEST_CASE(conversion_from_string_t_to_string)
{
    std::string nominal{ "Hello World" };
    value_t gen{ string_t{ nominal } };
    auto const actual = conversion_helper<std::string>::from_generic(gen);
    BOOST_REQUIRE_EQUAL(nominal, actual);
}

BOOST_AUTO_TEST_CASE(conversion_from_array_and_back)
{
    using array_t = std::array<int, 3>;

    array_t nominal{ 1, 2, 3 };
    auto generic = conversion_helper<array_t>::to_generic(nominal);
    BOOST_REQUIRE_NO_THROW(boost::get<sequence_t>(generic));

    const auto& actual = conversion_helper<array_t>::from_generic(generic);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(), nominal.cend(),
        actual.cbegin(), actual.cend());
}

BOOST_AUTO_TEST_CASE(conversion_from_array_with_invalid_value_size_throws)
{
    std::array<int, 3> long_array{ 1, 2, 3 };
    std::array<int, 2> short_array;

    BOOST_REQUIRE_THROW(
        conversion_helper<decltype(short_array)>::from_generic(conversion_helper<decltype(long_array)>::to_generic(long_array)),
        invalid_value_error);
}

using sequence_container_but_not_string_types = boost::mpl::list<
    std::vector<int>,
    std::deque<int>,
    std::forward_list<int>,
    std::list<int>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(conversion_from_sequence_container_but_not_string_and_back, T, sequence_container_but_not_string_types)
{
    T nominal{ 1, 2, 3 };
    auto generic = conversion_helper<T>::to_generic(nominal);
    BOOST_REQUIRE_NO_THROW(boost::get<sequence_t>(generic));

    const auto& actual = conversion_helper<T>::from_generic(generic);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(), nominal.cend(),
        actual.cbegin(), actual.cend());
}

BOOST_AUTO_TEST_CASE(conversion_from_array_with_binary_encoding)
{
    using array_t = std::array<int, 3>;

    array_t nominal{ 1, 2, 3 };
    auto generic = conversion_helper<array_t, encoding_hint::binary>::to_generic(nominal);
    BOOST_REQUIRE_NO_THROW(boost::get<binary_t>(boost::get<scalar_t>(generic)));

    const auto& actual = conversion_helper<array_t, encoding_hint::binary>::from_generic(generic);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(), nominal.cend(),
        actual.cbegin(), actual.cend());
}

using sequence_container_types = boost::mpl::list<
    std::string,
    std::vector<int>,
    std::deque<int>,
    std::forward_list<int>,
    std::list<int>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(sequence_container_to_binary_conversion, T, sequence_container_types)
{
    T nominal(3, 0);
    std::iota(nominal.begin(), nominal.end(), 0);

    auto generic = conversion_helper<T, encoding_hint::binary>::to_generic(nominal);
    BOOST_REQUIRE_NO_THROW(boost::get<binary_t>(boost::get<scalar_t>(generic)));

    const auto& actual = conversion_helper<T, encoding_hint::binary>::from_generic(generic);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(
        nominal.cbegin(), nominal.cend(),
        actual.cbegin(), actual.cend());
}

BOOST_AUTO_TEST_SUITE_END()
