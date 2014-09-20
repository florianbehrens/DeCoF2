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

#include <string>
#include <vector>

namespace decof
{

// Scalar parameter types
typedef bool boolean;
typedef int32_t integer;
typedef double real;
typedef std::string string;

struct binary : public std::string
{
    using std::string::string;
};

// Sequence parameter types
typedef std::vector<bool> boolean_seq;
typedef std::vector<int> integer_seq;
typedef std::vector<double> real_seq;
typedef std::vector<std::string> string_seq;
typedef std::vector<binary> binary_seq;

// Tuple parameter type
template<typename... Args>
struct tuple : public std::tuple<Args...>
{
    using std::tuple<Args...>::tuple;
};

} // namespace decof

#endif // DECOF_TYPES_H
