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
#include <vector>

#include <boost/any.hpp>

#include "types.h"

namespace decof
{

typedef std::vector<boost::any> dynamic_tuple;

template<typename Tuple>
struct TupleConversion
{
    static const size_t tuple_size = std::tuple_size<Tuple>::value;
};

template<typename Tuple, std::size_t N>
struct TupleConversionImpl : public TupleConversion<Tuple>
{
    static void tuple_to_dynamic(dynamic_tuple &arr, const Tuple &t)
    {
        TupleConversionImpl<Tuple, N-1>::tuple_to_dynamic(arr, t);
        arr[N-1] = boost::any(std::get<N-1>(t));
    }

    static void dynamic_to_tuple(Tuple &t, const dynamic_tuple &arr)
    {
        TupleConversionImpl<Tuple, N-1>::dynamic_to_tuple(t, arr);
        std::get<N-1>(t) = boost::any_cast<typename std::tuple_element<N-1, Tuple>::type>(arr[N-1]);
    }
};

template<typename Tuple>
struct TupleConversionImpl<Tuple, 1> : public TupleConversion<Tuple>
{
    static void tuple_to_dynamic(dynamic_tuple &arr, const Tuple &t)
    {
        arr[0] = boost::any(std::get<0>(t));
    }

    static void dynamic_to_tuple(Tuple &t, const dynamic_tuple &arr)
    {
        std::get<0>(t) = boost::any_cast<typename std::tuple_element<0, Tuple>::type>(arr[0]);
    }
};

template<typename... Elems>
void tuple_to_dynamic(dynamic_tuple &arr, const std::tuple<Elems...> &t)
{
    TupleConversionImpl<typename std::remove_reference<decltype(t)>::type, sizeof...(Elems)>::tuple_to_dynamic(arr, t);
}

template<typename... Elems>
void dynamic_to_tuple(std::tuple<Elems...> &t, const dynamic_tuple &a)
{
    TupleConversionImpl<std::tuple<Elems...>, sizeof...(Elems)>::dynamic_to_tuple(t, a);
}

// Generic conversions class for scalar and sequence types.
template<typename T>
struct Conversion
{
    inline static boost::any to_any(const T &value) {
        return boost::any(value);
    }

    inline static T from_any(const boost::any &any_value) {
        return boost::any_cast<T>(any_value);
    }
};

// Partial template specialization for conversion of tuple types.
template<typename... Args>
struct Conversion<decof::tuple<Args...>>
{
    typedef decof::tuple<Args...> tuple_type;

    inline static boost::any to_any(const tuple_type &value) {
        dynamic_tuple any_value(sizeof...(Args));
        tuple_to_dynamic(any_value, value);
        return boost::any(any_value);
    }

    inline static tuple_type from_any(const boost::any &any_value) {
        tuple_type value;
        dynamic_to_tuple(value, boost::any_cast<dynamic_tuple>(any_value));
        return value;
    }
};

} // namespace decof

#endif // DECOF_CONVERSION_H
