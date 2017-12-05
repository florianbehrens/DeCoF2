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

template<typename T>
using sequence = std::deque<T>;

/**
 * @brief Type tagging class.
 *
 * This template class creates a completely new type so that the following
 * condition is @c true:
 * @code
 * typeid(tag<T>) != typeid(T)
 * @endcode
 *
 * The new type behaves exactly like the original type, though.
 */
template<typename T, size_t Id = 0>
struct tag : public T
{
    using T::T;

    /**
     * @brief Explicit default constructor.
     *
     * We need an explicit default constructor, because it is not automatically
     * generated for classes with at least one user-defined constructor.
     */
    tag() = default;

    /**
     * @brief Implicit conversion from base type.
     */
    tag(const T& rhs) :
        T(rhs)
    {}

    /**
     * @brief Assignment operator from base type.
     */
    tag& operator=(const T& rhs)
    {
        if (static_cast<T*>(this) == &rhs) {
            return *this;
        } else {
            T::operator=(rhs);
        }

        return *this;
    }

    /**
     * @brief Move assignment operator from base type.
     */
    tag& operator=(T&& rhs)
    {
        if (static_cast<T*>(this) == &rhs) {
            return *this;
        } else {
            T::operator=(std::move(rhs));
        }

        return *this;
    }

    /**
     * @brief Move constructor from base type.
     */
    tag(T&& rhs) :
        T(std::move(rhs))
    {}
};

enum {
    string_tag,
    binary_tag
};

using string_t = tag<string, string_tag>;
using binary_t = tag<string, binary_tag>;

using scalar_t = boost::variant<boolean, integer, real, string_t, binary_t>;

enum {
    sequence_tag,
    tuple_tag
};

using sequence_t = tag<sequence<scalar_t>, sequence_tag>;
using tuple_t = tag<sequence<scalar_t>, tuple_tag>;
using value_t = boost::variant<scalar_t, sequence_t, tuple_t>;

} // namespace decof

#endif // DECOF_TYPES_H
