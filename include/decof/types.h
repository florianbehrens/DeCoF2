/*
 * Copyright (c) 2014 Florian Behrens
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

#ifndef DECOF_TYPES_H
#define DECOF_TYPES_H

#include <cctype>
#include <cmath>
#include <deque>
#include <limits>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/variant.hpp>

#include "exceptions.h"

namespace decof
{

// Scalar parameter types
typedef bool boolean;
typedef long long integer;
typedef double real;
typedef std::string string;

// TODO:
struct binary : public std::string
{
    using std::string::string;
};

template<typename T>
using sequence = std::deque<T>;

using generic_scalar = boost::variant<boolean, integer, real, string>;

template<typename T>
T convert_lossless_to_floating_point(integer i)
{
    auto const limit = (1ll << std::numeric_limits<T>::digits);

    if (i > limit || i < -limit)
        throw invalid_value_error();

    return static_cast<T>(i);
}

template<typename To, typename From>
inline To convert_lossless(const From& from)
{
    try {
        return boost::numeric_cast<To>(from);
    } catch (boost::bad_numeric_cast&) {
        throw invalid_value_error();
    }
}

template<typename T>
inline T convert_lossless_to_integral(real r)
{
    if (std::floor(r) == r) {
        try {
            return boost::numeric_cast<T>(r);
        } catch(boost::bad_numeric_cast&) {
            throw invalid_value_error();
        }
    } else {
        throw invalid_value_error();
    }
}

template<typename T, size_t Id>
struct tagged_type
{
    T value;
};

enum {
    sequence_tag,
    tuple_tag
};

using sequence_t = tagged_type<sequence<generic_scalar>, sequence_tag>;
using tuple_t = tagged_type<sequence<generic_scalar>, tuple_tag>;
using value_t = boost::variant<generic_scalar, sequence_t, tuple_t>;

/**
 * @brief Generic equality operator for tagged_type instances.
 */
template<typename T, size_t Id>
bool operator==(const tagged_type<T, Id>& lhs, const tagged_type<T, Id>& rhs)
{
    return lhs == rhs;
}

} // namespace decof

#endif // DECOF_TYPES_H
