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
#include <string>
#include <vector>

#include <boost/any.hpp>
#include <boost/variant.hpp>

namespace decof
{

// Scalar parameter types
typedef bool boolean;
typedef std::intmax_t integer;
typedef double real;
typedef std::string string;

struct binary : public std::string
{
    using std::string::string;
};





typedef std::vector<boolean> boolean_seq;
typedef std::vector<integer> integer_seq;
typedef std::vector<real> real_seq;
typedef std::vector<string> string_seq;
typedef std::vector<binary> binary_seq;

typedef boost::any tuple;

/// Variant type capable of holding any DeCoF value type.
typedef boost::variant<
    boolean, integer, real, string, binary,                     // scalar types
    boolean_seq, integer_seq, real_seq, string_seq, binary_seq, // sequence types
    tuple> value_t;




// Sequence parameter types
//template<typename T>
//struct sequence : public std::vector<T>
//{
//    using std::vector<T>::vector;
//};

//typedef sequence<bool> boolean_seq;
//typedef sequence<std::intmax_t> integer_seq;
//typedef sequence<double> real_seq;
//typedef sequence<std::string> string_seq;
//typedef sequence<binary> binary_seq;

// Tuple parameter type
//template<typename... Args>
//struct tuple : public std::tuple<Args...>
//{
//    using std::tuple<Args...>::tuple;
//};

} // namespace decof

#endif // DECOF_TYPES_H
