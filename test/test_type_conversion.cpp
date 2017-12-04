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

BOOST_AUTO_TEST_CASE(empty_value_t)
{
    value_t empty;
    BOOST_REQUIRE_THROW(conversion_helper<int>::from_generic(empty), wrong_type_error);
}

BOOST_AUTO_TEST_CASE(integral_type_covertible_to_floating_point)
{
    const integer nominal = (1ll << std::numeric_limits<float>::digits) - 1;

    value_t val_min{ -nominal };
    BOOST_REQUIRE_EQUAL(conversion_helper<float>::from_generic(val_min), -nominal);

    value_t val_max{ nominal };
    BOOST_REQUIRE_EQUAL(conversion_helper<float>::from_generic(val_max), nominal);
}

BOOST_AUTO_TEST_CASE(floating_point_type_convertible_to_integral)
{
    const integer nominal = (1ll << std::numeric_limits<real>::digits) - 1;

    value_t val_min{ static_cast<real>(-nominal) };
    BOOST_REQUIRE_EQUAL(conversion_helper<integer>::from_generic(val_min), -nominal);

    value_t val_max{ static_cast<real>(nominal) };
    BOOST_REQUIRE_EQUAL(conversion_helper<integer>::from_generic(val_max), nominal);
}

BOOST_AUTO_TEST_SUITE_END()
