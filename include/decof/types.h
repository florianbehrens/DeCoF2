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

template<typename T, size_t Id>
struct tagged_type
{
    T value;
};

/**
 * @brief Generic equality operator for tagged_type instances.
 */
template<typename T, size_t Id>
bool operator==(const tagged_type<T, Id>& lhs, const tagged_type<T, Id>& rhs)
{
    return lhs == rhs;
}

enum {
    sequence_tag,
    tuple_tag
};

using scalar_t = boost::variant<boolean, integer, real, string>;
using sequence_t = tagged_type<sequence<scalar_t>, sequence_tag>;
using tuple_t = tagged_type<sequence<scalar_t>, tuple_tag>;
using value_t = boost::variant<scalar_t, sequence_t, tuple_t>;

} // namespace decof

#endif // DECOF_TYPES_H
