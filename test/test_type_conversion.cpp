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

// TODO
//#include "value.h"
#include <decof/types.h>
#include <decof/conversion.h>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include <iostream>

#include "test_helpers.h"

using namespace decof;

BOOST_AUTO_TEST_SUITE(type_conversion)

using decof_types = boost::mpl::list<boolean, integer, real, string, sequence<boolean>, sequence<integer>, sequence<real>, sequence<string>>;

BOOST_AUTO_TEST_CASE_TEMPLATE(round_trip_conversion, T, decof_types)
{
    auto nominal = T();
    auto actual  = conversion_helper<T>::from_generic(conversion_helper<T>::to_generic(nominal));

    BOOST_REQUIRE(actual == nominal);
}

#if 0
using integral_types = boost::mpl::list<char, unsigned char, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

BOOST_AUTO_TEST_CASE_TEMPLATE(integer_type_within_bounds, T, integral_types)
{
    value_t var{ 0 };
    BOOST_REQUIRE_EQUAL(value_t_cast<T>(var), 0);
}

BOOST_AUTO_TEST_CASE(integral_type)
{
    value_t var{ integer(0ll) };
    value_t var_under{ integer(std::numeric_limits<int>::min() - 1ll) };
    value_t var_min{ integer(std::numeric_limits<int>::min()) };
    value_t var_max{ integer(std::numeric_limits<int>::max()) };
    value_t var_over{ integer(std::numeric_limits<int>::max() + 1ll) };

    BOOST_REQUIRE_EQUAL(value_t_cast<double>(var), 0.0);
    BOOST_REQUIRE_THROW(value_t_cast<int>(var_under), decof::invalid_value_error);
    BOOST_REQUIRE_EQUAL(value_t_cast<int>(var_min), std::numeric_limits<int>::min());
    BOOST_REQUIRE_EQUAL(value_t_cast<int>(var_max), std::numeric_limits<int>::max());
    BOOST_REQUIRE_THROW(value_t_cast<int>(var_over), decof::invalid_value_error);
}

BOOST_AUTO_TEST_CASE(unsigned_integral_type)
{
    value_t var_min{ integer(0) };
    value_t var_max{ integer(static_cast<long long>(std::numeric_limits<unsigned int>::max())) };
    value_t var_over{ integer(static_cast<long long>(std::numeric_limits<unsigned int>::max() + 1ll)) };

    BOOST_REQUIRE_EQUAL(value_t_cast<unsigned int>(var_min), std::numeric_limits<unsigned int>::min());
    BOOST_REQUIRE_EQUAL(value_t_cast<unsigned int>(var_max), std::numeric_limits<unsigned int>::max());
    BOOST_REQUIRE_THROW(value_t_cast<unsigned int>(var_over), decof::invalid_value_error);
}

BOOST_AUTO_TEST_CASE(floating_point_type)
{
    value_t var{ 1.23 };
    value_t var_under{ std::numeric_limits<float>::lowest() - 1e23 };
    value_t var_min{ std::numeric_limits<float>::lowest() };
    value_t var_max{ std::numeric_limits<float>::max() };
    value_t var_over{ std::numeric_limits<float>::max() + 1e23 };
    value_t var_integer{ 0ll };

    BOOST_REQUIRE_THROW(value_t_cast<integer>(var), decof::invalid_value_error);
    BOOST_REQUIRE_THROW(value_t_cast<float>(var_under), decof::invalid_value_error);
    BOOST_REQUIRE_EQUAL(value_t_cast<float>(var_min), std::numeric_limits<float>::lowest());
    BOOST_REQUIRE_EQUAL(value_t_cast<float>(var_max), std::numeric_limits<float>::max());
    BOOST_REQUIRE_THROW(value_t_cast<float>(var_over), decof::invalid_value_error);
    BOOST_REQUIRE_EQUAL(value_t_cast<double>(var_integer), 0.0);
}

BOOST_AUTO_TEST_CASE(floating_point_type_convertible_to_integer)
{
    const auto nominal = (1ll << std::numeric_limits<real>::digits) - 1;

    value_t var{ 1.0 };
    value_t var_min{ static_cast<real>(-nominal) };
    value_t var_max{ static_cast<real>(nominal) };

    BOOST_REQUIRE_EQUAL(value_t_cast<integer>(var), 1);
    BOOST_REQUIRE_EQUAL(value_t_cast<integer>(var_min), -nominal);
    BOOST_REQUIRE_EQUAL(value_t_cast<integer>(var_max), nominal);
}

BOOST_AUTO_TEST_CASE(string_type)
{
    std::string nominal("Hellp World");
    value_t var{ decof::conversion_helper<decltype(nominal)>::to_generic(nominal) };
    BOOST_REQUIRE_EQUAL(value_t_cast<std::string>(var), nominal);
}

BOOST_AUTO_TEST_CASE(vector_of_floats)
{
    std::vector<float> nominal{
        std::numeric_limits<float>::lowest(),
        0.0,
        std::numeric_limits<float>::max() };

    value_t var{ decof::conversion_helper<decltype(nominal)>::to_generic(nominal) };
    BOOST_REQUIRE(value_t_cast<std::vector<float>>(var) == nominal);
}

BOOST_AUTO_TEST_CASE(boolean_sequence)
{
    sequence<bool> bool_sequence{ false, true };
    value_t var = conversion_helper<sequence<bool>>::to_generic(bool_sequence);

    auto actual_var = conversion_helper<sequence<bool>>::from_generic(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), bool_sequence.cbegin(), bool_sequence.cend());

    auto var_moved = conversion_helper<sequence<bool>>::to_generic(std::move(bool_sequence));
    BOOST_REQUIRE_EQUAL(bool_sequence.size(), 0);

    actual_var = conversion_helper<sequence<bool>>::from_generic(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), bool_sequence.cbegin(), bool_sequence.cend());
}

BOOST_AUTO_TEST_CASE(integer_sequence_convertible_to_real_sequence)
{
    const auto digits = std::min(std::numeric_limits<double>::digits, std::numeric_limits<long long int>::digits);

    const value_t var{ sequence_t{ std::deque<generic_scalar>{
        integer(1ll << (digits - 1)),
        integer(0),
        -integer(1ll << (digits - 1))
    } } };

    auto const actual = value_t_cast<sequence<real>>(var);
    auto const nominal = value_t_cast<sequence<integer>>(var);

    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual.cbegin(), actual.cend(), nominal.cbegin(), nominal.cend());
}

BOOST_AUTO_TEST_CASE(integer_sequence)
{
    sequence<integer> integer_sequence{ 0ll, 1ll };
    value_t var{ sequence_t{ integer_sequence } };

    auto actual_var = value_t_cast<sequence<long long>>(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), integer_sequence.cbegin(), integer_sequence.cend());

    value_t var_moved{ std::move(integer_sequence) };
    BOOST_REQUIRE_EQUAL(integer_sequence.size(), 0);

    auto actual_var_moved = value_t_cast<sequence<long long>>(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var_moved.cbegin(), actual_var_moved.cend(), actual_var.cbegin(), actual_var.cend());
}

BOOST_AUTO_TEST_CASE(real_sequence)
{
    sequence<real> real_sequence{ std::numeric_limits<real>::min(), 0.0, std::numeric_limits<real>::max() };
    value_t var{ real_sequence };

    auto actual_var = value_t_cast<sequence<real>>(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), real_sequence.cbegin(), real_sequence.cend());

    value_t var_moved{ std::move(real_sequence) };
    BOOST_REQUIRE_EQUAL(real_sequence.size(), 0);

    auto actual_var_moved = value_t_cast<sequence<double>>(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var_moved.cbegin(), actual_var_moved.cend(), actual_var.cbegin(), actual_var.cend());
}

BOOST_AUTO_TEST_CASE(real_sequence_convertible_to_integer_sequence)
{
    const auto real_max = (1ll << std::numeric_limits<real>::digits) - 1;

    sequence<real> real_sequence{
        static_cast<real>(-real_max),
        0.0,
        static_cast<real>(real_max) };
    value_t var{ real_sequence };

    auto const actual = value_t_cast<sequence<integer>>(var);
    auto const nominal = value_t_cast<sequence<real>>(var);

    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual.cbegin(), actual.cend(), nominal.cbegin(), nominal.cend());
}

BOOST_AUTO_TEST_CASE(string_sequence)
{
    sequence<string> string_sequence{ "", "Hello World" };
    value_t var{ string_sequence };

    auto actual_var = value_t_cast<sequence<string>>(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), string_sequence.cbegin(), string_sequence.cend());

    value_t var_moved{ std::move(string_sequence) };
    BOOST_REQUIRE_EQUAL(string_sequence.size(), 0);

    auto actual_var_moved = value_t_cast<sequence<string>>(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var_moved.cbegin(), actual_var_moved.cend(), actual_var.cbegin(), actual_var.cend());
}

BOOST_AUTO_TEST_CASE(tuple)
{
    auto nominal = std::make_tuple(false, 0ll, 0.0, std::string("abc"));
    value_t var{ decof::conversion_helper<decltype(nominal)>::to_generic(nominal) };

    BOOST_REQUIRE(value_t_cast<decltype(nominal)>(var) == nominal);
}
#endif
BOOST_AUTO_TEST_SUITE_END()
