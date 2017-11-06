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

#ifndef VALUE_H
#define VALUE_H

#include <cmath>
#include <memory>
#include <tuple>
#include <type_traits>

#include <decof/exceptions.h>
#include <decof/types.h>

#include <boost/noncopyable.hpp>
#include <boost/variant.hpp>

// -------

#include <decof/conversion.h>

namespace decof {

template<typename T>
constexpr bool integral_not_bool = std::is_integral<T>::value && !std::is_same<T, bool>::value;

template<typename T>
using enable_if_bool = typename std::enable_if<std::is_same<T, bool>::value, bool>::type;

template<typename T>
using enable_if_integral_not_bool = typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type;

template<typename T>
using enable_if_unsigned_long_long = typename std::enable_if<std::is_same<T, unsigned long long>::value, unsigned long long>::type;

template<typename T>
using enable_if_floating_point = typename std::enable_if<std::is_floating_point<T>::value, T>::type;

//template<typename T>
//using enable_if_not_arithmetic = typename std::enable_if<!std::is_arithmetic<T>::value, T>::type;

template<typename T, typename U>
using enable_if_same = typename std::enable_if<std::is_same<T, U>::value, T>::type;

template<typename T>
using enable_if_not_arithmetic_and_not_floating_point_sequence = typename std::enable_if<!std::is_arithmetic<T>::value && !std::is_floating_point<typename T::value_type>::value, T>::type;

using scalar_variant = boost::variant<bool, integer, real, string/*, binary*/>;

using variant_base = boost::variant<
    bool,
    integer,
    real,
    string,
//    binary,
    sequence<bool>,
    sequence<integer>,
    sequence<real>,
    sequence<string>,
//    sequence<binary>,
    sequence<scalar_variant>
>;

/**
 * @brief A type-safe type-erasing value container for DeCoP value types.
 *
 * bool <-> bool.
 * long long:
 *     <-> All integral types if the value can be converted lossless.
 *      -> All floating point types
 * double <-> All floating point types.
 * std::string <-> std::string.
 */
class variant : public variant_base
{
public:
    template<typename T>
    explicit variant(T val, enable_if_bool<T> = false) :
        variant_base(val)
    {}

    /**
     * @brief Constructor for unsigned long long types.
     *
     * @throws invalid_value_error exception if given argument does not fit
     * into a long long type without loss.
     */
    template<typename T>
    explicit variant(T val, enable_if_unsigned_long_long<T> = 0) :
        variant_base(static_cast<long long>(val <= std::numeric_limits<long long>::max() ?
                                            val : throw invalid_value_error()))
    {}

    /**
     * @brief Constructor for all integral types but bool and unsigned long
     * long.
     */
    template<typename T>
    explicit variant(T val, enable_if_integral_not_bool<T> = 0) :
        variant_base(static_cast<long long>(val))
    {}


//    template<typename T>
//    explicit variant(typename std::enable_if<std::is_same<T, bool>::value, bool>::type val) :
//        m_value(val)
//    {}

//    explicit variant(short val) :
//        m_value(static_cast<long long>(val))
//    {}

//    explicit variant(int val) :
//        m_value(static_cast<long long>(val))
//    {}

//    explicit variant(long val) :
//        m_value(static_cast<long long>(val))
//    {}

//    explicit variant(long long val) :
//        m_value(val)
//    {}

//    template<typename T>
//    explicit variant(typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value, T>::type val) :
//        m_value(static_cast<long long>(val))
//    {}

    /**
     * @brief Constructor for floating point types.
     */
    explicit variant(double val) :
        variant_base(val)
    {}

    explicit variant(const char* val) :
        variant_base(std::string(val))
    {}

    explicit variant(const std::string& val) :
        variant_base(val)
    {}

//    explicit variant(const sequence<bool>& val) :
//        variant_base(val)
//    {}

    template<typename T>
    explicit variant(const sequence<T>& val) :
        variant_base(val)
    {}

    template<typename T>
    explicit variant(sequence<T>&& val) :
        variant_base(std::move(val))
    {}

//    template<typename T>
//    explicit variant(T&& val) :
//        variant_base(std::forward<T>(val))
//    {}

private:
};

// Forward declaration
template<typename T>
T variant_cast(const scalar_variant& var);

namespace detail {

template<typename T>
T to_floating_point(integer val)
{
    auto const limit = (int64_t(1) << (std::numeric_limits<T>::digits - 1));

    if (val > limit || val < -limit)
        throw invalid_value_error();

    return static_cast<T>(val);
}

// TODO: Better to use boost::numeric_cast<>?
template<typename To, typename From>
To convert_lossless_or_throw(const From& from)
{
    if (from > std::numeric_limits<To>::max() ||
        from < std::numeric_limits<To>::lowest())
        throw invalid_value_error();

    return static_cast<To>(from);
}

/**
 * @brief Conversion helper class.
 *
 * @tparam T The target type for the conversion.
 *
 * A helper class for conversions of decof::variant types to concrete types is
 * needed in order to make use of partial template specialization which is not
 * possible with function templates.
 */
template<typename T, typename Enable = void>
struct variant_cast_helper
{
    static T cast(const variant& var) try {
        return boost::get<T>(var);
    } catch (boost::bad_get&) {
        throw wrong_type_error();
    }
};

/**
 * @brief Partial template specialization for integral types except bool.
 */
template<typename T>
struct variant_cast_helper<
    T,
    typename std::enable_if<
        std::is_integral<T>::value && !std::is_same<T, bool>::value
    >::type
>
{
    static T cast(const variant& var) try {
        if (var.type() == typeid(real)) {
            auto val = boost::get<real>(var);

            if (std::floor(val) == val) {
                return static_cast<T>(val);
            } else {
                throw invalid_value_error();
            }
        } else {
            return convert_lossless_or_throw<T>(boost::get<integer>(var));
        }
    } catch (boost::bad_get&) {
        throw wrong_type_error();
    }
};

/**
 * @brief Partial template specialization for floating point types.
 */
template<typename T>
struct variant_cast_helper<
    T,
    typename std::enable_if<
        std::is_floating_point<T>::value
    >::type
>
{
    static T cast(const variant& var) try {
        if (var.type() == typeid(integer)) {
            return to_floating_point<T>(boost::get<integer>(var));
        } else {
            return convert_lossless_or_throw<T>(boost::get<real>(var));
        }
    } catch (boost::bad_get&) {
        throw wrong_type_error();
    }
};

/**
 * @brief Partial template specialization for sequence of integral except bool
 * types.
 */
template<typename T>
struct variant_cast_helper<
    sequence<T>,
    typename std::enable_if<
        std::is_integral<T>::value && !std::is_same<T, bool>::value
    >::type
>
{
    static sequence<T> cast(const variant& var) {
        sequence<T> retval;

        if (var.type() == typeid(sequence<real>)) {
            auto const& val = boost::get<sequence<real>>(var);
            retval.reserve(val.size());

            for (auto const& elem : val) {
                if (std::floor(elem) == elem) {
                    retval.push_back(convert_lossless_or_throw<T>(elem));
                } else {
                    throw invalid_value_error();
                }
            }
        } else {
            try {
                auto const& val = boost::get<sequence<integer>>(var);
                retval.reserve(val.size());

                for (auto const& elem : val) {
                    retval.push_back(convert_lossless_or_throw<T>(elem));
                }
            } catch (boost::bad_get&) {
                throw wrong_type_error();
            }
        }

        return retval;
    }
};

/**
 * @brief Partial template specialization for sequence of floating point types.
 */
template<typename T>
struct variant_cast_helper<
    sequence<T>,
    typename std::enable_if<
        std::is_floating_point<T>::value
    >::type
>
{
    static sequence<T> cast(const variant& var) {
        sequence<T> retval;

        if (var.type() == typeid(sequence<integer>)) {
            auto const& val = boost::get<sequence<integer>>(var);
            retval.reserve(val.size());

            for (auto const& elem : val) {
                retval.push_back(to_floating_point<T>(elem));
            }
        } else {
            try {
                auto const& val = boost::get<sequence<real>>(var);
                retval.reserve(val.size());

                for (auto const& elem : val) {
                    retval.push_back(convert_lossless_or_throw<T>(elem));
                }
            } catch (boost::bad_get&) {
                throw wrong_type_error();
            }
        }

        return retval;
    }
};

/**
 * @brief Partial template specialization for tuple types.
 */
template<typename... Args>
struct variant_cast_helper<std::tuple<Args...>>
{
    typedef std::tuple<Args...> tuple_type;

    template<typename Tuple>
    struct tuple_conversion
    {
        static const size_t tuple_size = std::tuple_size<Tuple>::value;
    };

    template<typename Tuple, std::size_t N>
    struct tuple_conversion_impl : public tuple_conversion<Tuple>
    {
//        static void to_variant(dynamic_tuple &arr, const Tuple &t)
//        {
//            tuple_conversion_impl<Tuple, N-1>::to_variant(arr, t);
//            arr[N-1] = boost::any(std::get<N-1>(t));
//        }

        static void from_variant(Tuple &to, const sequence<scalar_variant>& from)
        {
            tuple_conversion_impl<Tuple, N-1>::from_variant(to, from);
            std::get<N-1>(to) = boost::get<typename std::tuple_element<N-1, Tuple>::type>(from[N-1]);
        }
    };

    template<typename Tuple>
    struct tuple_conversion_impl<Tuple, 1> : public tuple_conversion<Tuple>
    {
//        static void to_variant(dynamic_tuple &arr, const Tuple &t)
//        {
//            arr[0] = boost::any(std::get<0>(t));
//        }

        static void from_variant(Tuple &to, const sequence<scalar_variant>& from)
        {
            std::get<0>(to) = boost::get<typename std::tuple_element<0, Tuple>::type>(from[0]);
        }
    };

//    template<typename... Elems>
//    void to_variant(dynamic_tuple &arr, const std::tuple<Elems...> &t)
//    {
//        tuple_conversion_impl<typename std::remove_reference<decltype(t)>::type, sizeof...(Elems)>::to_variant(arr, t);
//    }

    template<typename... Elems>
    static void from_variant(std::tuple<Elems...>& to, const sequence<scalar_variant>& from)
    {
        tuple_conversion_impl<std::tuple<Elems...>, sizeof...(Elems)>::from_variant(to, from);
    }

    static tuple_type cast(const variant& var) {
        tuple_type value;
        from_variant(value, boost::get<sequence<scalar_variant>>(var));
        return value;
    }
};

} // namespace detail

template<typename T>
T variant_cast(const variant& var)
{
    return detail::variant_cast_helper<T>::cast(var);
}

template<typename T>
T variant_cast(const scalar_variant& var)
{
    return detail::variant_cast_helper<T>::cast(var);
}

#if 0
/**
 * @brief Wraps a type in order to prevent from implicit conversions into
 * another type.
 */
template<typename T>
class explicit_wrapper
{
public:
    explicit_wrapper() = default;
    explicit_wrapper(const explicit_wrapper&) = default;
    explicit_wrapper(explicit_wrapper&&) = default;

    explicit explicit_wrapper(const T& value) :
        m_value(value)
    {}

    explicit explicit_wrapper(T&& value) :
        m_value(std::move(value))
    {}

    explicit_wrapper& operator=(const T& rhs)
    {
        m_value = rhs;
        return *this;
    }

    explicit_wrapper& operator=(T&& rhs)
    {
        m_value = std::move(rhs);
        return *this;
    }

    explicit operator const T&() const
    {
        return m_value;
    }

private:
    T m_value;
};

using binary2 = std::unique_ptr<uint8_t[]>;

/**
 * @brief Variant cast function for bool types.
 */
template<typename T>
enable_if_bool<T> variant_cast(const decof::variant& var) try
{
//    return static_cast<bool>(boost::get<explicit_wrapper<bool>>(var));
    return boost::get<bool>(var);
} catch (boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for integral (except bool) types.
 */
template<typename T>
enable_if_integral_not_bool<T> variant_cast(const decof::variant& var) try
{
    return convert_lossless_or_throw<T>(boost::get<integer>(var));
} catch (boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for floating point types.
 */
template<typename T>
enable_if_floating_point<T> variant_cast(const decof::variant& var) try
{
    if (var.type() == typeid(integer))
        return to_floating_point<T>(boost::get<integer>(var));
    else
        return convert_lossless_or_throw<T>(boost::get<real>(var));
} catch (boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for std::string types.
 */
template<typename T>
enable_if_same<T, std::string> variant_cast(const decof::variant& var) try
{
    return boost::get<std::string>(var);
} catch(boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for sequence of bool types.
 */
template<typename T>
enable_if_same<T, sequence<boolean>> variant_cast(const decof::variant& var) try
{
    return boost::get<sequence<boolean>>(var);
} catch(boost::bad_get&) {
    throw decof::wrong_type_error();
}

// TODO
/**
 * @brief Variant cast function for sequence of integer types.
 */
template<typename template<typename<T> class>
typename std::enable_if<integral_not_bool<T>, sequence<T>>::type
variant_cast(const decof::variant& var) {  // enable_if_integral_not_bool<T>
    sequence<T> retval;
    auto const& val = boost::get<sequence<integer>>(var);
    retval.reserve(val.size());

    for (auto const& elem : val) {
        retval.push_back(elem);
    }

    return retval;
}

template<typename T>
enable_if_same<T, sequence<integer>> variant_cast(const decof::variant& var) try
{
    return boost::get<sequence<integer>>(var);
} catch(boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for sequence of floating point types.
 */
template<typename T>
typename std::enable_if<
//    std::is_floating_point<typename T::value_type>::value,
    std::is_same<typename T::value_type, float>::value,
    T
>::type
variant_cast(const decof::variant& var) try
{
    sequence<float> retval;

    // Special case for sequence of integer
    if (var.type() == typeid(sequence<integer>)) {
        auto const& val = boost::get<sequence<integer>>(var);
        retval.reserve(val.size());

        for (auto const v : val)
            retval.push_back(to_floating_point<float>(v));
    } else {
        auto const& val = boost::get<sequence<real>>(var);
        retval.reserve(val.size());

        for (auto const v : val)
            retval.push_back(convert_lossless_or_throw<float>(v));
    }

    return retval;
} catch (boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for sequence of real types.
 */
template<typename T>
enable_if_same<T, sequence<real>> variant_cast(const decof::variant& var) try
{
    return boost::get<sequence<real>>(var);
} catch (boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for sequence of string types.
 */
template<typename T>
enable_if_same<T, sequence<string>> variant_cast(const decof::variant& var) try
{
    return boost::get<sequence<string>>(var);
} catch (boost::bad_get&) {
    throw decof::wrong_type_error();
}

/**
 * @brief Variant cast function for all other types.
 */
//template<typename T>
//enable_if_not_arithmetic_and_not_floating_point_sequence<T> variant_cast(const decof::variant& var)
//{
//    try {
//        return boost::get<T>(var);
//    } catch (boost::bad_get&) {
//        throw decof::wrong_type_error();
//    }
//}
#endif // 0

/**
 * @brief Variant cast function for tuples
 */
#if 0
template<typename... Args>
std::tuple<Args...> variant_cast(const decof::variant& var)
{
    try {
        std::tuple<Args...> retval;

        TupleConversionImpl<
            std::tuple<Args...>,
            sizeof...(Args)>::dynamic_to_tuple(
                retval,
                boost::get<dynamic_tuple>(var));

        return retval;
    } catch (boost::bad_any_cast&) {
        throw decof::wrong_type_error();
    }
}
#endif

class encoding_visitor : public boost::static_visitor<>
{
public:
    explicit encoding_visitor(std::ostream& out) :
        m_out(out)
    {}

    void operator()(const bool& val) const
    {
        m_out << std::boolalpha << val << std::noboolalpha;
    }

    void operator()(const integer& val) const
    {
        m_out << val;
    }

    void operator()(const real& val) const
    {
        m_out << val;
    }

    void operator()(const std::string& val) const
    {
        m_out << val;
    }

//    void operator()(const binary& val) const
//    {
//        m_out;
//    }

    void operator()(const sequence<bool>& val) const
    {
        m_out << '[';

        for (auto it = val.cbegin(); it != val.cend(); ++it) {
            if (it != val.cbegin())
                m_out << ",";
            this->operator()(*it);
        }

        m_out << '[';
    }

    void operator()(const sequence<integer>& val) const
    {
        m_out << '[';

        for (auto it = val.cbegin(); it != val.cend(); ++it) {
            if (it != val.cbegin())
                m_out << ",";
            this->operator()(*it);
        }

        m_out << '[';
    }

    void operator()(const sequence<real>& val) const
    {
        m_out << '[';

        for (auto it = val.cbegin(); it != val.cend(); ++it) {
            if (it != val.cbegin())
                m_out << ",";
            this->operator()(*it);
        }

        m_out << '[';
    }

    void operator()(const sequence<string>& val) const
    {
        m_out << '[';

        for (auto it = val.cbegin(); it != val.cend(); ++it) {
            if (it != val.cbegin())
                m_out << ",";
            this->operator()(*it);
        }

        m_out << '[';
    }

//    void operator()(const sequence<binary>& val) const
//    {
//        m_out;
//    }

    void operator()(const boost::any& val) const
    {
        m_out;
    }

private:
    std::ostream& m_out;
};

////////////////////////////////////////////////////////////////////////
// Solution based on partial template specialization (with helper class)

#if 0
template<typename T>
void foo();

template<typename T>
struct C
{
 static constexpr const char* type = "fallback";

 void operator()() {
     std::cout << "C::operator(): " << type << std::endl;
 }
};

template<>
struct C<bool>
{
 static constexpr const char* type = "bool";

 void operator()() {
     std::cout << "C::operator(): " << type << std::endl;
 }
};

template<>
struct C<int>
{
 static constexpr const char* type = "int";

 void operator()() {
     std::cout << "C::operator(): " << type << std::endl;
 }
};

template<typename T>
struct C<std::vector<T>>
{
 static constexpr const char* type = "vector<T>";

 void operator()() {
     std::cout << "C::operator(): " << type << ", ";
     foo<T>();
 }
};

template<typename T>
void foo() {
 C<T>()();
}

int main()
{
 foo<bool>();
 foo<int>();
 foo<std::vector<int>>();

}

#endif // 0

} // namespace decof

#endif // VALUE_H
