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

#include "encoding_hint.h"
#include "exceptions.h"
#include "types.h"
#include <boost/iterator/transform_iterator.hpp>
#include <tuple>
#include <type_traits>

namespace decof {

/**
 * @brief Converts an integer number losslessly to type T or throws.
 *
 * @param i Integer value to convert losslessly to type T.
 * @throw invalid_value_error if conversion is not lossless.
 */
template <typename T>
T convert_lossless_to_floating_point(integer_t i)
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
template <typename To, typename From>
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
template <typename T>
inline T convert_lossless_to_integral(real_t r)
{
    if (std::floor(r) == r) {
        try {
            return boost::numeric_cast<T>(r);
        } catch (boost::bad_numeric_cast&) {
            throw invalid_value_error();
        }
    } else {
        throw invalid_value_error();
    }
}

/**
 * @brief Helper class for conversion from/to a #scalar_t type.
 *
 * @tparam T The target type for the conversion.
 * @tparam EncodingHint Encoding hints for use by client contexts.
 * @tparam Enable Dummy argument to enable partial template specialization.
 *
 * A helper class for conversions between the type #scalar_t and
 * the corresponding concrete type and vice versa is needed in order to
 * make use of partial template specialization which is not possible with
 * function templates.
 *
 * The following table shows the available specializations and the
 * corresponding #scalar_t's alternative type:
 *
 * <table>
 *   <tr>
 *     <th>#scalar_t alternative type
 *     <th>Encoding hint
 *     <th>Type requirements of @c T
 *   <tr>
 *     <td>@c bool
 *     <td>
 *     <td>@c T is @c bool
 *   <tr>
 *     <td>@c intmax_t
 *     <td>
 *     <td>@c T is integral but not @c bool
 *   <tr>
 *     <td>@c double
 *     <td>
 *     <td>@c T is a floating point type
 *   <tr>
 *     <td>@c string_t
 *     <td>
 *     <td>@c T is @c std\::string
 *   <tr>
 *     <td>@c binary_t
 *     <td>binary
 *     <td>@c T is @c std\::string
 *   <tr>
 *     <td>@c string_t
 *     <td>
 *     <td>@c T is <tt>const char*</tt> (one-way conversion to #scalar_t only)
 *   <tr>
 *     <td>@c binary_t
 *     <td>@c binary
 *     <td>@c T is <tt>const char*</tt> (one-way conversion to #scalar_t only)
 *   <tr>
 *     <td>@c binary_t
 *     <td>@c binary
 *     <td>@c T is @c std::array<>
 *   <tr>
 *     <td>@c binary_t
 *     <td>@c binary
 *     <td>@c T meets the requirements of a @c
 *         <a href="http://en.cppreference.com/w/cpp/concept/SequenceContainer">SequenceContainer</a>
 *         (not including @c std::array<>) but not @c std::string
 *   <tr>
 *     <td>@c binary_t
 *     <td>@c binary
 *     <td>All other @c T
 * </table>
 */
template <typename T, encoding_hint EncodingHint = encoding_hint::none, typename Enable = void>
struct scalar_conversion_helper
{
    using type_tag = binary_tag;

    /**
     * @brief Conversion from value of type generic_scalar to concrete type.
     *
     * @throws wrong_type_error in case of a type mismatch.
     */
    static T from_generic(const scalar_t& arg)
    {
        if (arg.type() != typeid(binary_t))
            throw wrong_type_error();

        auto const& binary = boost::get<binary_t>(arg);

        T retval;
        std::copy_n(binary.cbegin(), sizeof(T), reinterpret_cast<T*>(&retval));
        return retval;
    }

    /**
     * @brief Conversion from concrete to value of type generic_scalar.
     *
     * @throws invalid_value_error if the conversion is not possible without
     * loss of precision.
     */
    static scalar_t to_generic(const T& arg)
    {
        return binary_t(reinterpret_cast<const char*>(&arg), reinterpret_cast<const char*>(&arg) + sizeof(arg));
    }
};

/**
 * @brief Partial template specialization for type @c bool.
 */
template <>
struct scalar_conversion_helper<bool, encoding_hint::none>
{
    using type_tag = boolean_tag;

    static bool from_generic(const scalar_t& arg)
    {
        if (arg.type() != typeid(boolean_t))
            throw wrong_type_error();

        return boost::get<boolean_t>(arg);
    }

    /**
     * @brief Conversion from concrete to value of type generic_scalar.
     *
     * @throws invalid_value_error if the conversion is not possible without
     * loss of precision.
     */
    static scalar_t to_generic(const bool& arg)
    {
        return scalar_t{arg};
    }
};

/**
 * @brief Partial template specialization for integral types except bool.
 */
template <typename T>
struct scalar_conversion_helper<
    T,
    encoding_hint::none,
    typename std::enable_if<
        std::is_integral<typename std::remove_reference<T>::type>::value &&
        !std::is_same<typename std::remove_reference<T>::type, bool>::value>::type>
{
    using type_tag = integer_tag;

    static T from_generic(const scalar_t& var)
    {
        if (var.type() == typeid(real_t)) {
            return convert_lossless_to_integral<T>(boost::get<real_t>(var));
        } else if (var.type() == typeid(integer_t)) {
            return convert_lossless<T>(boost::get<integer_t>(var));
        }

        throw wrong_type_error();
    }

    static scalar_t to_generic(const T& arg)
    {
        return scalar_t{convert_lossless<integer_t>(arg)};
    }
};

/**
 * @brief Partial template specialization for floating point types.
 */
template <typename T>
struct scalar_conversion_helper<
    T,
    encoding_hint::none,
    typename std::enable_if<std::is_floating_point<typename std::remove_reference<T>::type>::value>::type>
{
    using type_tag = real_tag;

    static T from_generic(const scalar_t& var)
    {
        if (var.type() == typeid(integer_t)) {
            return convert_lossless_to_floating_point<T>(boost::get<integer_t>(var));
        } else if (var.type() == typeid(real_t)) {
            return convert_lossless<T>(boost::get<real_t>(var));
        }

        throw wrong_type_error();
    }

    static scalar_t to_generic(const T& arg)
    {
        return scalar_t{convert_lossless<real_t>(arg)};
    }
};

/**
 * @brief Partial template specialization for type <tt>const char*</tt>.
 *
 * This specialization only provides a conversion from the concrete to the
 * generic type, but not vice verse. Hence the type <tt>const char*</tt> can
 * only be used with readonly parameters.
 */
template <>
struct scalar_conversion_helper<const char*, encoding_hint::none>
{
    using type_tag = string_tag;

    static scalar_t to_generic(const char* arg)
    {
        return string_t(arg);
    }
};

/**
 * @brief Partial template specialization for binary encoding of type
 * <tt>const char*</tt>.
 *
 * This specialization only provides a conversion from the concrete to the
 * generic type, but not vice verse. Hence the type <tt>const char*</tt> can
 * only be used with readonly parameters.
 */
template <>
struct scalar_conversion_helper<const char*, encoding_hint::binary>
{
    using type_tag = binary_tag;

    static scalar_t to_generic(const char* arg)
    {
        return binary_t(arg);
    }
};

/**
 * @brief Partial template specialization for type @c std::string.
 */
template <>
struct scalar_conversion_helper<std::string, encoding_hint::none>
{
    using type_tag = string_tag;

    static std::string from_generic(const scalar_t& arg)
    {
        if (arg.type() != typeid(string_t)) {
            throw wrong_type_error();
        }

        return boost::get<string_t>(arg);
    }

    static scalar_t to_generic(const std::string& arg)
    {
        return string_t{arg};
    }
};

/**
 * @brief Partial template specialization for type @c std::string and binary
 * encoding.
 */
template <>
struct scalar_conversion_helper<std::string, encoding_hint::binary>
{
    using type_tag = binary_tag;

    static std::string from_generic(const scalar_t& arg)
    {
        if (arg.type() == typeid(string_t)) {
            return boost::get<string_t>(arg);
        } else if (arg.type() == typeid(binary_t)) {
            return boost::get<binary_t>(arg);
        } else {
            throw wrong_type_error();
        }
    }

    static scalar_t to_generic(const std::string& arg)
    {
        return binary_t{arg};
    }
};

/**
 * @brief Partial template specialization for type @c std::array<> and binary
 * encoding.
 */
template <typename T, size_t N>
struct scalar_conversion_helper<std::array<T, N>, encoding_hint::binary>
{
    using type_tag = binary_tag;

    static std::array<T, N> from_generic(const scalar_t& arg)
    {
        if (arg.type() != typeid(binary_t))
            throw wrong_type_error();

        const auto& binary = boost::get<binary_t>(arg);

        if (binary.size() != N * sizeof(T))
            throw invalid_value_error();

        std::array<T, N> retval;
        std::copy(
            reinterpret_cast<const T*>(binary.data()),
            reinterpret_cast<const T*>(binary.data() + binary.size()),
            retval.data());
        return retval;
    }

    static scalar_t to_generic(const std::array<T, N>& arg)
    {
        return binary_t(
            reinterpret_cast<const char*>(arg.data()), reinterpret_cast<const char*>(arg.data() + arg.size()));
    }
};

/**
 * @brief Partial template specialization for types meeting (some of) the
 * requirements of a @c
 * <a href="http://en.cppreference.com/w/cpp/concept/SequenceContainer">SequenceContainer</a>
 * (not including @c std::array<>) but not @c std::string and binary encoding.
 */
template <typename T>
struct scalar_conversion_helper<
    T,
    encoding_hint::binary,
    typename std::enable_if<
        std::is_constructible<
            T,
            sequence_t::const_iterator, // Should be a boost::transform_iterator!
            sequence_t::const_iterator  // Should be a boost::transform_iterator!
            >::value &&
        std::is_same< // has T::const_iterator T::cbegin() method?
            decltype(std::declval<T>().cbegin()),
            typename T::const_iterator>::value &&
        std::is_same< // has T::const_iterator T::cend() method?
            decltype(std::declval<T>().cend()),
            typename T::const_iterator>::value &&
        !std::is_same<T, std::string>::value>::type>
{
    using type_tag = binary_tag;

    static T from_generic(const scalar_t& arg)
    {
        if (arg.type() != typeid(binary_t))
            throw wrong_type_error();

        auto const& binary = boost::get<binary_t>(arg);

        if (binary.size() % sizeof(typename T::value_type) != 0)
            throw invalid_value_error();

        return T(
            reinterpret_cast<const typename T::value_type*>(binary.data()),
            reinterpret_cast<const typename T::value_type*>(binary.data() + binary.size()));
    }

    static scalar_t to_generic(const T& arg)
    {
        binary_t retval;
        retval.reserve(std::distance(arg.cbegin(), arg.cend()) * sizeof(typename T::value_type));

        auto const out_it = std::back_inserter(retval);
        for (auto const& elem : arg) {
            std::copy_n(reinterpret_cast<const char*>(&elem), sizeof(typename T::value_type), out_it);
        }

        return retval;
    }
};

/**
 * @brief Helper class for conversion from/to a value_t.
 *
 * @tparam T The target type for the conversion.
 * @tparam EncodingHint Encoding hints for use by client contexts.
 * @tparam Enable Dummy argument to enable partial template specialization.
 *
 * A helper class for conversions between generic (e.g., decof::variant) and
 * the corresponding concrete type and vice versa is needed in order to make
 * use of partial template specialization which is not possible with function
 * templates.
 *
 * The following table shows the available specializations and the
 * corresponding #value_t's alternative type:
 *
 * <table>
 *   <tr>
 *     <th>#value_t alternative type
 *     <th>Encoding hint
 *     <th>Type requirements of @c T
 *   <tr>
 *     <td>@c #sequence_t
 *     <td>
 *     <td>@c T is @c std::array<>
 *   <tr>
 *     <td>@c #sequence_t
 *     <td>
 *     <td>@c T meets (some of) the requirements of @c
 *         <a href="http://en.cppreference.com/w/cpp/concept/SequenceContainer">SequenceContainer</a>,
 *         but is not @c std::string
 *   <tr>
 *     <td>@c #tuple_t
 *     <td>
 *     <td>T is a @c std::tuple<>
 * </table>
 *
 * The following standard containers meet the requirements of
 * @c <a href="http://en.cppreference.com/w/cpp/concept/SequenceContainer">SequenceContainer</a>:
 * @c std::string, @c std::vector, @c std::deque, @c std::forward_list, and
 * @c std::list.
 */
template <typename T, encoding_hint EncodingHint = encoding_hint::none, typename Enable = void>
struct conversion_helper
{
    using type_tag = typename scalar_conversion_helper<T, EncodingHint>::type_tag;

    /**
     * @brief Conversion from generic (e.g., decof::variant) to concrete type.
     *
     * @throws wrong_type_error in case of a type mismatch.
     */
    static T from_generic(const value_t& arg)
    {
        if (arg.type() != typeid(scalar_t))
            throw wrong_type_error();

        return scalar_conversion_helper<T, EncodingHint>::from_generic(boost::get<scalar_t>(arg));
    }

    /**
     * @brief Conversion from concrete to generic (e.g., decof::variant) type.
     *
     * @throws invalid_value_error if the conversion is not possible without
     * loss of precision.
     */
    static value_t to_generic(const T& arg)
    {
        return value_t{scalar_conversion_helper<T, EncodingHint>::to_generic(arg)};
    }
};

/**
 * @brief Partial template specialization for type @c std::array<>.
 */
template <typename T, size_t N>
struct conversion_helper<std::array<T, N>, encoding_hint::none>
{
    using type_tag = sequence_tag<typename scalar_conversion_helper<T, encoding_hint::none>::type_tag>;

    static std::array<T, N> from_generic(const value_t& arg)
    {
        if (arg.type() != typeid(sequence_t))
            throw wrong_type_error();

        const auto& val = boost::get<sequence_t>(arg);
        if (val.size() != N)
            throw invalid_value_error();

        std::array<T, N> retval;
        std::transform(val.cbegin(), val.cend(), retval.begin(), scalar_conversion_helper<T>::from_generic);

        return retval;
    }

    static value_t to_generic(const std::array<T, N>& arg)
    {
        sequence_t tmp;
        std::transform(arg.cbegin(), arg.cend(), std::back_inserter(tmp), scalar_conversion_helper<T>::to_generic);
        return std::move(tmp);
    }
};

/**
 * @brief Partial template specialization for types meeting the requirements of
 * a @c
 * <a href="http://en.cppreference.com/w/cpp/concept/SequenceContainer">SequenceContainer</a>
 * but not @c std::string.
 */
template <typename T>
struct conversion_helper<
    T,
    encoding_hint::none,
    typename std::enable_if<
        std::is_constructible<
            T,
            sequence_t::const_iterator, // Should be a boost::transform_iterator!
            sequence_t::const_iterator  // Should be a boost::transform_iterator!
            >::value &&
        std::is_same< // has T::const_iterator T::cbegin() method?
            decltype(std::declval<T>().cbegin()),
            typename T::const_iterator>::value &&
        std::is_same< // has T::const_iterator T::cbegin() method?
            decltype(std::declval<T>().cend()),
            typename T::const_iterator>::value &&
        !std::is_same<T, std::string>::value>::type>
{
    using type_tag = sequence_tag<typename scalar_conversion_helper<typename T::value_type>::type_tag>;

    static T from_generic(const value_t& arg)
    {
        if (arg.type() != typeid(sequence_t))
            throw wrong_type_error();

        auto const& src       = boost::get<sequence_t>(arg);
        auto const& transform = scalar_conversion_helper<typename T::value_type>::from_generic;

        return T(
            boost::make_transform_iterator(src.cbegin(), transform),
            boost::make_transform_iterator(src.cend(), transform));
    }

    static value_t to_generic(const T& arg)
    {
        auto const& transform = scalar_conversion_helper<typename T::value_type>::to_generic;

        return sequence_t(
            boost::make_transform_iterator(arg.cbegin(), transform),
            boost::make_transform_iterator(arg.cend(), transform));
    }
};

/**
 * @brief Partial template specialization for tuple types.
 */
template <typename... Args>
struct conversion_helper<std::tuple<Args...>, encoding_hint::none>
{
    using type_tag   = tuple_tag;
    using value_type = std::tuple<Args...>;

    template <typename Tuple>
    struct basic_tuple_conversion_helper
    {
        static const size_t tuple_size = std::tuple_size<Tuple>::value;
    };

    template <typename Tuple, std::size_t N>
    struct tuple_conversion_helper : public basic_tuple_conversion_helper<Tuple>
    {
        static void from_generic(Tuple& to, const tuple_t& from)
        {
            tuple_conversion_helper<Tuple, N - 1>::from_generic(to, from);
            std::get<N - 1>(to) =
                scalar_conversion_helper<typename std::tuple_element<N - 1, Tuple>::type>::from_generic(from[N - 1]);
        }

        static void to_generic(tuple_t& to, const Tuple& from)
        {
            tuple_conversion_helper<Tuple, N - 1>::to_generic(to, from);
            to.push_back(scalar_conversion_helper<typename std::tuple_element<N - 1, Tuple>::type>::to_generic(
                std::get<N - 1>(from)));
        }
    };

    template <typename Tuple>
    struct tuple_conversion_helper<Tuple, 1> : public basic_tuple_conversion_helper<Tuple>
    {
        static void from_generic(Tuple& to, const tuple_t& from)
        {
            std::get<0>(to) =
                scalar_conversion_helper<typename std::tuple_element<0, Tuple>::type>::from_generic(from[0]);
        }

        static void to_generic(tuple_t& to, const Tuple& from)
        {
            to.push_back(
                scalar_conversion_helper<typename std::tuple_element<0, Tuple>::type>::to_generic(std::get<0>(from)));
        }
    };

    template <typename... Elems>
    static void from_generic(std::tuple<Elems...>& to, const tuple_t& from)
    {
        tuple_conversion_helper<std::tuple<Elems...>, sizeof...(Elems)>::from_generic(to, from);
    }

    template <typename... Elems>
    static void to_generic(tuple_t& to, const std::tuple<Elems...>& from)
    {
        tuple_conversion_helper<std::tuple<Elems...>, sizeof...(Elems)>::to_generic(to, from);
    }

    static value_type from_generic(const value_t& arg)
    {
        if (arg.type() != typeid(tuple_t))
            throw wrong_type_error();

        value_type retval;
        from_generic(retval, boost::get<tuple_t>(arg));
        return retval;
    }

    static value_t to_generic(const value_type& arg)
    {
        tuple_t retval;
        to_generic(retval, arg);
        return retval;
    }
};

} // namespace decof

#endif // DECOF_CONVERSION_H
