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

#include <boost/numeric/conversion/cast.hpp>
#include <boost/variant.hpp>
#include <cctype>
#include <cmath>
#include <deque>
#include <limits>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>

#include "exceptions.h"

namespace decof {

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
template <typename T, size_t Id = 0>
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
    tag(const T& rhs) : T(rhs)
    {
    }

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
    tag(T&& rhs) : T(std::move(rhs))
    {
    }
};

/**
 * @name Tags for DeCoP parameter type identification.
 * @{
 */
struct boolean_tag
{
};
struct integer_tag
{
};
struct real_tag
{
};
struct string_tag
{
};
struct binary_tag
{
};
template <typename T>
struct sequence_tag
{
};
struct tuple_tag
{
};
///@}

/**
 * @name DeCoP parameter type aliases.
 * @{
 */
using boolean_t  = bool;
using integer_t  = long long;
using real_t     = double;
using string_t   = tag<std::string, 0>;
using binary_t   = tag<std::string, 1>;
using scalar_t   = boost::variant<boolean_t, integer_t, real_t, string_t, binary_t>;
using sequence_t = tag<std::deque<scalar_t>, 0>;
using tuple_t    = tag<std::deque<scalar_t>, 1>;
///@}

/**
 * @brief Type safe union for exchange of parameter values.
 */
using value_t = boost::variant<scalar_t, sequence_t, tuple_t>;

} // namespace decof

#endif // DECOF_TYPES_H
