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

// TODO
#include "value.h"

#include <iostream>

using namespace decof;

BOOST_AUTO_TEST_SUITE(binary)

//BOOST_AUTO_TEST_CASE(bin1)
//{
//    decof::binary2 b{ new uint8_t[4] };
//}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(type_conversion)

using decof::variant_cast;

BOOST_AUTO_TEST_CASE(sizeof_variant)
{
    std::cout << "sizeof(decof::variant) / sizeof(double) = " << sizeof(variant) / sizeof(double) << std::endl;
    std::cout << "sizeof(string) = " << sizeof(string) << std::endl;
    std::cout << "sizeof(sequence<string>) = " << sizeof(sequence<string>) << std::endl;
}

BOOST_AUTO_TEST_CASE(boolean_type)
{
    decof::variant var{ true };

    BOOST_REQUIRE_EQUAL(variant_cast<bool>(var), true);

    std::ostringstream out;
    boost::apply_visitor(decof::encoding_visitor(out), var);

    BOOST_REQUIRE_EQUAL(out.str(), "true");

    BOOST_REQUIRE_THROW(variant_cast<integer>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<double>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<std::string>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<bool>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<long long>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<double>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<std::string>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

}

BOOST_AUTO_TEST_CASE(integer_type_within_bounds)
{
    decof::variant var{ 0 };

    BOOST_REQUIRE_EQUAL(variant_cast<short>(var), 0);
    BOOST_REQUIRE_EQUAL(variant_cast<unsigned short>(var), 0);
    BOOST_REQUIRE_EQUAL(variant_cast<int>(var), 0);
    BOOST_REQUIRE_EQUAL(variant_cast<unsigned int>(var), 0);
    BOOST_REQUIRE_EQUAL(variant_cast<long int>(var), 0);
    BOOST_REQUIRE_EQUAL(variant_cast<unsigned long int>(var), 0);
    BOOST_REQUIRE_EQUAL(variant_cast<long long int>(var), 0);
    BOOST_REQUIRE_EQUAL(variant_cast<unsigned long long int>(var), 0);
}

BOOST_AUTO_TEST_CASE(integral_type)
{
    decof::variant var{ 0ll };
    decof::variant var_under{ std::numeric_limits<int>::min() - 1ll };
    decof::variant var_min{ std::numeric_limits<int>::min() };
    decof::variant var_max{ std::numeric_limits<int>::max() };
    decof::variant var_over{ std::numeric_limits<int>::max() + 1ll };

    BOOST_REQUIRE_THROW(variant_cast<bool>(var), decof::wrong_type_error);
    BOOST_REQUIRE_EQUAL(variant_cast<double>(var), 0.0);
    BOOST_REQUIRE_THROW(variant_cast<std::string>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<bool>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<long long>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<double>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<std::string>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

    BOOST_REQUIRE_THROW(variant_cast<int>(var_under), decof::invalid_value_error);
    BOOST_REQUIRE_EQUAL(variant_cast<int>(var_min), std::numeric_limits<int>::min());
    BOOST_REQUIRE_EQUAL(variant_cast<int>(var_max), std::numeric_limits<int>::max());
    BOOST_REQUIRE_THROW(variant_cast<int>(var_over), decof::invalid_value_error);
}

BOOST_AUTO_TEST_CASE(unsigned_integral_type)
{
    decof::variant var_min{ 0 };
    decof::variant var_max{ static_cast<long long>(std::numeric_limits<unsigned int>::max()) };
    decof::variant var_over{ static_cast<long long>(std::numeric_limits<unsigned int>::max() + 1ll) };

    BOOST_REQUIRE_EQUAL(variant_cast<unsigned int>(var_min), std::numeric_limits<unsigned int>::min());
    BOOST_REQUIRE_EQUAL(variant_cast<unsigned int>(var_max), std::numeric_limits<unsigned int>::max());
    BOOST_REQUIRE_THROW(variant_cast<unsigned int>(var_over), decof::invalid_value_error);
}

BOOST_AUTO_TEST_CASE(floating_point_type)
{
    decof::variant var{ 1.23 };
    decof::variant var_under{ std::numeric_limits<float>::lowest() - 1e23 };
    decof::variant var_min{ std::numeric_limits<float>::lowest() };
    decof::variant var_max{ std::numeric_limits<float>::max() };
    decof::variant var_over{ std::numeric_limits<float>::max() + 1e23 };
    decof::variant var_integer{ 0ll };

    BOOST_REQUIRE_THROW(variant_cast<bool>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<integer>(var), decof::invalid_value_error);
    BOOST_REQUIRE_THROW(variant_cast<std::string>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<bool>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<integer>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<real>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<std::string>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

    BOOST_REQUIRE_THROW(variant_cast<float>(var_under), decof::invalid_value_error);
    BOOST_REQUIRE_EQUAL(variant_cast<float>(var_min), std::numeric_limits<float>::lowest());
    BOOST_REQUIRE_EQUAL(variant_cast<float>(var_max), std::numeric_limits<float>::max());
    BOOST_REQUIRE_THROW(variant_cast<float>(var_over), decof::invalid_value_error);

    BOOST_REQUIRE_EQUAL(variant_cast<double>(var_integer), 0.0);
}

BOOST_AUTO_TEST_CASE(floating_point_type_convertible_to_integer)
{
    const auto nominal = (1ll << std::numeric_limits<real>::digits) - 1;

    decof::variant var{ 1.0 };
    decof::variant var_min{ static_cast<real>(-nominal) };
    decof::variant var_max{ static_cast<real>(nominal) };

    BOOST_REQUIRE_EQUAL(variant_cast<integer>(var), 1);
    BOOST_REQUIRE_EQUAL(variant_cast<integer>(var_min), -nominal);
    BOOST_REQUIRE_EQUAL(variant_cast<integer>(var_max), nominal);
}

BOOST_AUTO_TEST_CASE(string_type)
{
    decof::variant var{ "Hello world" };
    decof::variant var2{ std::string("Hello world") };

    BOOST_REQUIRE_THROW(variant_cast<bool>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<long long>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<double>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<bool>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<long long>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<double>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<std::string>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

    BOOST_REQUIRE_EQUAL(variant_cast<std::string>(var), variant_cast<std::string>(var2));
}

BOOST_AUTO_TEST_CASE(boolean_sequence)
{
    sequence<bool> bool_sequence{ false, true };
    decof::variant var{ bool_sequence };

    BOOST_REQUIRE_THROW(variant_cast<bool>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<long long>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<double>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<std::string>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<long long>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<double>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<std::string>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

    auto actual_var = variant_cast<sequence<bool>>(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), bool_sequence.cbegin(), bool_sequence.cend());

    decof::variant var_moved{ std::move(bool_sequence) };
    BOOST_REQUIRE_EQUAL(bool_sequence.size(), 0);

    auto actual_var_moved = variant_cast<sequence<bool>>(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var_moved.cbegin(), actual_var_moved.cend(), actual_var.cbegin(), actual_var.cend());
}

BOOST_AUTO_TEST_CASE(integer_sequence_convertible_to_real_sequence)
{
    std::cout << "digits<double> = " << std::numeric_limits<double>::digits << std::endl;
    std::cout << "digits<long long int> = " << std::numeric_limits<long long int>::digits << std::endl;

    const auto digits = std::min(std::numeric_limits<double>::digits, std::numeric_limits<long long int>::digits);

    const variant var{ sequence<integer>{
        integer(1ll << (digits - 1)),
        0ll,
        -integer(1ll << (digits - 1)) }};

    auto const actual = variant_cast<sequence<real>>(var);
    auto const nominal = variant_cast<sequence<integer>>(var);

    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual.cbegin(), actual.cend(), nominal.cbegin(), nominal.cend());
}

BOOST_AUTO_TEST_CASE(integer_sequence)
{
    sequence<long long> integer_sequence{ 0ll, 1ll };
    decof::variant var{ integer_sequence };

    BOOST_REQUIRE_THROW(variant_cast<bool>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<long long>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<double>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<std::string>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<bool>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<std::string>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

    auto actual_var = variant_cast<sequence<long long>>(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), integer_sequence.cbegin(), integer_sequence.cend());

    decof::variant var_moved{ std::move(integer_sequence) };
    BOOST_REQUIRE_EQUAL(integer_sequence.size(), 0);

    auto actual_var_moved = variant_cast<sequence<long long>>(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var_moved.cbegin(), actual_var_moved.cend(), actual_var.cbegin(), actual_var.cend());
}

BOOST_AUTO_TEST_CASE(real_sequence)
{
    sequence<real> real_sequence{ std::numeric_limits<real>::min(), 0.0, std::numeric_limits<real>::max() };
    decof::variant var{ real_sequence };

    BOOST_REQUIRE_THROW(variant_cast<bool>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<integer>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<real>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<std::string>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<bool>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<integer>>(var), decof::invalid_value_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<std::string>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

    auto actual_var = variant_cast<sequence<real>>(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), real_sequence.cbegin(), real_sequence.cend());

    decof::variant var_moved{ std::move(real_sequence) };
    BOOST_REQUIRE_EQUAL(real_sequence.size(), 0);

    auto actual_var_moved = variant_cast<sequence<double>>(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var_moved.cbegin(), actual_var_moved.cend(), actual_var.cbegin(), actual_var.cend());
}

BOOST_AUTO_TEST_CASE(real_sequence_convertible_to_integer_sequence)
{
    const auto real_max = (1ll << std::numeric_limits<real>::digits) - 1;

    sequence<real> real_sequence{
        static_cast<real>(-real_max),
        0.0,
        static_cast<real>(real_max) };
    decof::variant var{ real_sequence };

    auto const actual = variant_cast<sequence<integer>>(var);
    auto const nominal = variant_cast<sequence<real>>(var);

    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual.cbegin(), actual.cend(), nominal.cbegin(), nominal.cend());
}

BOOST_AUTO_TEST_CASE(string_sequence)
{
    sequence<string> string_sequence{ "", "Hello World" };
    decof::variant var{ string_sequence };

    BOOST_REQUIRE_THROW(variant_cast<bool>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<integer>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<real>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<string>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<binary>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<bool>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<integer>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<real>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<sequence<binary>>(var), decof::wrong_type_error);
//    BOOST_REQUIRE_THROW(variant_cast<tuple>(var), decof::wrong_type_error);

    auto actual_var = variant_cast<sequence<string>>(var);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var.cbegin(), actual_var.cend(), string_sequence.cbegin(), string_sequence.cend());

    decof::variant var_moved{ std::move(string_sequence) };
    BOOST_REQUIRE_EQUAL(string_sequence.size(), 0);

    auto actual_var_moved = variant_cast<sequence<string>>(var_moved);
    BOOST_REQUIRE_EQUAL_COLLECTIONS(actual_var_moved.cbegin(), actual_var_moved.cend(), actual_var.cbegin(), actual_var.cend());
}

BOOST_AUTO_TEST_CASE(legal_tuple)
{
//    using tuple_t = std::tuple<boolean, integer, real, string>;

    auto nominal = std::make_tuple(false, 0ll, 0.0, std::string("abc"));
    decof::variant var{ sequence<scalar_variant> {
        std::get<0>(nominal),
        std::get<1>(nominal),
        std::get<2>(nominal),
        std::get<3>(nominal)
    } };

    BOOST_REQUIRE_THROW(variant_cast<boolean>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<integer>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<real>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<string>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<boolean>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<integer>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<real>>(var), decof::wrong_type_error);
    BOOST_REQUIRE_THROW(variant_cast<sequence<string>>(var), decof::wrong_type_error);

    BOOST_REQUIRE(variant_cast<decltype(nominal)>(var) == nominal);
}

BOOST_AUTO_TEST_SUITE_END()
