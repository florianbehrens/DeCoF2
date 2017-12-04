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

#ifndef DECOF_CONVERSION_H
#define DECOF_CONVERSION_H

#include <tuple>
#include <type_traits>

#include "exceptions.h"
#include "types.h"

namespace decof
{

/**
 * @brief Converts an integer number losslessly to type T or throws.
 *
 * @param i Integer value to convert losslessly to type T.
 * @throw invalid_value_error if conversion is not lossless.
 */
template<typename T>
T convert_lossless_to_floating_point(integer i)
{
    auto const limit = (1ll << std::numeric_limits<T>::digits);

    if (i > limit || i < -limit)
        throw invalid_value_error();

    return static_cast<T>(i);
}

/**
 * @brief Converts an input type From losslessly to type To or throws.
 *
 * @param from Value to convert losslessly to type To.
 * @throw invalid_value_error if conversion is not lossless.
 */
template<typename To, typename From>
inline To convert_lossless(const From& from)
{
    try {
        return boost::numeric_cast<To>(from);
    } catch (boost::bad_numeric_cast&) {
        throw invalid_value_error();
    }
}

/**
 * @brief Converts a real value losslessly to an integer value or throws.
 *
 * @param r Real value to convert losslessly to type T.
 * @throw invalid_value_error if conversion is not lossless.
 */
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

/**
 * @brief Helper class for conversion from/to generic_scalar.
 *
 * @tparam T The target type for the conversion.
 *
 * A helper class for conversions between a generic scalar type and
 * the corresponding concrete type and vice versa is needed in order to make
 * use of partial template specialization which is not possible with function
 * templates.
 */
template<typename T, typename Enable = void>
struct scalar_conversion_helper
{
    /**
     * @brief Conversion from value of type generic_scalar to concrete type.
     *
     * @throws wrong_type_error in case of a type mismatch.
     */
    static T from_generic(const scalar_t& arg) {
        if (arg.type() != typeid(T))
            throw wrong_type_error();

        return boost::get<T>(arg);
    }

    /**
     * @brief Conversion from concrete to value of type generic_scalar.
     *
     * @throws invalid_value_error if the conversion is not possible without
     * loss of precision.
     */
    static scalar_t to_generic(const T& arg) {
        return scalar_t{ arg };
    }
};

/**
 * @brief Partial template specialization for integral types except bool.
 */
template<typename T>
struct scalar_conversion_helper<
    T,
    typename std::enable_if<
        std::is_integral<typename std::remove_reference<T>::type>::value &&
        !std::is_same<typename std::remove_reference<T>::type, bool>::value
    >::type
>
{
    static T from_generic(const scalar_t& var) {
        if (var.type() == typeid(real)) {
            return convert_lossless_to_integral<T>(boost::get<real>(var));
        } else if (var.type() == typeid(integer)) {
            return convert_lossless<T>(boost::get<integer>(var));
        }

        throw wrong_type_error();
    }

    static scalar_t to_generic(const T& arg) {
        return scalar_t{ convert_lossless<integer>(arg) };
    }
};

/**
 * @brief Partial template specialization for floating point types.
 */
template<typename T>
struct scalar_conversion_helper<
    T,
    typename std::enable_if<
        std::is_floating_point<typename std::remove_reference<T>::type>::value
    >::type
>
{
    static T from_generic(const scalar_t& var) {
        if (var.type() == typeid(integer)) {
            return convert_lossless_to_floating_point<T>(boost::get<integer>(var));
        } else if (var.type() == typeid(real)) {
            return convert_lossless<T>(boost::get<real>(var));
        }

        throw wrong_type_error();
    }

    static scalar_t to_generic(const T& arg) {
        return scalar_t{ convert_lossless<real>(arg) };
    }
};

/**
 * @brief Partial template specialization for sequence of char types.
 */
template<typename T>
struct scalar_conversion_helper<
    T,
    typename std::enable_if<
        std::is_same<decltype(std::declval<const T>().data()), typename T::value_type const*>::value && // has T::data() method?
        std::is_same<decltype(std::declval<T>().size()), typename T::size_type>::value && // has T::size() method?
        std::is_constructible<T, typename T::value_type const*, typename T::value_type const*>::value // has T(It, It) constructor
    >::type
>
{
    static T from_generic(const scalar_t& arg) {
        if (arg.type() != typeid(std::string)) {
            throw wrong_type_error();
        }

        auto const& str = boost::get<std::string>(arg);

        if (str.size() % sizeof(typename T::value_type)) {
            throw invalid_value_error();
        }

        return T(
            reinterpret_cast<typename T::value_type const*>(&str[0]),
            reinterpret_cast<typename T::value_type const*>(&str[str.size()])
        );
    }

    static scalar_t to_generic(const T& arg) {
        std::string tmp(
            reinterpret_cast<const char*>(arg.data()),
            reinterpret_cast<const char*>(arg.data() + arg.size()));
        return scalar_t{ std::move(tmp) };
    }
};

/**
 * @brief Helper class for conversion from/to a value_t.
 *
 * @tparam T The target type for the conversion.
 * @tparam Enable Dummy argument to enable partial template specialization.
 *
 * A helper class for conversions between generic (e.g., decof::variant) and
 * the corresponding concrete type and vice versa is needed in order to make
 * use of partial template specialization which is not possible with function
 * templates.
 */
template<typename T, typename Enable = void>
struct conversion_helper
{
    /**
     * @brief Conversion from generic (e.g., decof::variant) to concrete type.
     *
     * @throws wrong_type_error in case of a type mismatch.
     */
    static T from_generic(const value_t& arg) {
        if (arg.type() != typeid(scalar_t))
            throw wrong_type_error();

        return scalar_conversion_helper<T>::from_generic(boost::get<scalar_t>(arg));
    }

    /**
     * @brief Conversion from concrete to generic (e.g., decof::variant) type.
     *
     * @throws invalid_value_error if the conversion is not possible without
     * loss of precision.
     */
    static value_t to_generic(const T& arg) {
        return value_t{ scalar_conversion_helper<T>::to_generic(arg) };
    }
};

/**
 * @brief Partial template specialization for sequence types.
 */
template<typename T>
struct conversion_helper<sequence<T>>
{
    using value_type = sequence<T>;

    static value_type from_generic(const value_t& arg) {
        if (arg.type() != typeid(sequence_t))
            throw wrong_type_error();

        value_type retval;
        for (auto const& elem : boost::get<sequence_t>(arg).value) {
            retval.push_back(scalar_conversion_helper<T>::from_generic(elem));
        }

        return retval;
    }

    static value_t to_generic(const value_type& arg) {
        sequence_t tmp;
        for (auto const& elem : arg) {
            tmp.value.push_back(scalar_conversion_helper<T>::to_generic(elem));
        }

        return value_t{ std::move(tmp) };
    }
};

/**
 * @brief Partial template specialization for tuple types.
 */
template<typename... Args>
struct conversion_helper<std::tuple<Args...>>
{
    using value_type = std::tuple<Args...>;

    template<typename Tuple>
    struct basic_tuple_conversion_helper
    {
        static const size_t tuple_size = std::tuple_size<Tuple>::value;
    };

    template<typename Tuple, std::size_t N>
    struct tuple_conversion_helper : public basic_tuple_conversion_helper<Tuple>
    {
        static void from_generic(Tuple &to, const tuple_t& from)
        {
            tuple_conversion_helper<Tuple, N-1>::from_generic(to, from);
            std::get<N-1>(to) =
                scalar_conversion_helper<
                    typename std::tuple_element<N-1, Tuple>::type
                >::from_generic(from.value[N-1]);
        }

        static void to_generic(tuple_t& to, const Tuple& from)
        {
            tuple_conversion_helper<Tuple, N-1>::to_generic(to, from);
            to.value.push_back(
                scalar_conversion_helper<
                    typename std::tuple_element<N-1, Tuple>::type
                >::to_generic(std::get<N-1>(from)));
        }
    };

    template<typename Tuple>
    struct tuple_conversion_helper<Tuple, 1> : public basic_tuple_conversion_helper<Tuple>
    {
        static void from_generic(Tuple &to, const tuple_t& from)
        {
            std::get<0>(to) = scalar_conversion_helper<
                typename std::tuple_element<0, Tuple>::type
            >::from_generic(from.value[0]);
        }

        static void to_generic(tuple_t& to, const Tuple& from)
        {
            to.value.push_back(
                scalar_conversion_helper<
                    typename std::tuple_element<0, Tuple>::type
                >::to_generic(std::get<0>(from)));
        }
    };

    template<typename... Elems>
    static void from_generic(std::tuple<Elems...>& to, const tuple_t& from)
    {
        tuple_conversion_helper<std::tuple<Elems...>, sizeof...(Elems)>::from_generic(to, from);
    }

    template<typename... Elems>
    static void to_generic(tuple_t& to, const std::tuple<Elems...>& from)
    {
        tuple_conversion_helper<std::tuple<Elems...>, sizeof...(Elems)>::to_generic(to, from);
    }

    static value_type from_generic(const value_t& arg) {
        if (arg.type() != typeid(tuple_t))
            throw wrong_type_error();

        value_type retval;
        from_generic(retval, boost::get<tuple_t>(arg));
        return retval;
    }

    static value_t to_generic(const value_type& arg) {
        tuple_t retval;
        to_generic(retval, arg);
        return retval;
    }
};

} // namespace decof

#endif // DECOF_CONVERSION_H
