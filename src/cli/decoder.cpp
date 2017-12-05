/*
 * Copyright (c) 2016 Florian Behrens
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

#include "decoder.h"

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

namespace decof
{

namespace cli
{

std::string base64decode(std::string base64)
{
    // This function is based on code from
    // http://stackoverflow.com/questions/10521581/base64-encode-using-boost-throw-exception/10973348#10973348
    using namespace boost::archive::iterators;

    typedef transform_width<binary_from_base64<remove_whitespace<std::string::const_iterator>>, 8, 6> it_binary_t;

    unsigned int paddChars = count(base64.begin(), base64.end(), '=');
    std::replace(base64.begin(), base64.end(), '=', 'A');
    std::string result(it_binary_t(base64.begin()), it_binary_t(base64.end()));
    result.erase(result.end() - paddChars, result.end());
    return result;
}

} // namespace cli

} // namespace decof
