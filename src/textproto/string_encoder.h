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

#ifndef STRING_ENCODER_H
#define STRING_ENCODER_H

#include <string>

// Forward declaration
namespace boost {
class any;
}

namespace decof
{

struct string_encoder
{
    static std::string encode(const boost::any &any_value);
};

} // namespace decof

#endif // STRING_ENCODER_H
