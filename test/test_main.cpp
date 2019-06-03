/*
 * Copyright (c) 2015 Florian Behrens
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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE "decof2"

#include "test_helpers.h"
#include <decof/types.h>
#include <boost/test/unit_test.hpp>
#include <variant>

namespace {

struct visitor
{
    template <typename T>
    void operator()(const T& arg)
    {
        out << arg;
    }

    std::ostream& out;
};

} // namespace

namespace decof {

std::ostream& operator<<(std::ostream& out, const value_t& arg)
{
    std::visit(visitor{out}, arg);
    return out;
}

std::ostream& operator<<(std::ostream& out, const sequence_t& arg)
{
    out << '[';

    auto it = std::cbegin(arg);
    for (; it != std::cend(arg); ++it) {
        if (it != std::cbegin(arg))
            out.put(',');
        out << *it;
    }

    out << ']';

    return out;
}

std::ostream& operator<<(std::ostream& out, const tuple_t& arg)
{
    out << '{';

    auto it = std::cbegin(arg);
    for (; it != std::cend(arg); ++it) {
        if (it != std::cbegin(arg))
            out.put(',');
        out << *it;
    }

    out << '}';

    return out;
}

std::ostream& operator<<(std::ostream& out, const scalar_t& arg)
{
    std::visit(visitor{out}, arg);
    return out;
}

} // namespace decof
