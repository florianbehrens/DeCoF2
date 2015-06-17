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

#include "encoder.h"

#include <boost/any.hpp>

#include "conversion.h"
#include "exceptions.h"
#include "types.h"

namespace decof
{

void encoder::encode(std::ostream &out, const boolean &value)
{
    if (value == true)
        out << "#t";
    else
        out << "#f";
}

void encoder::encode(std::ostream &out, const string &value)
{
    out << "\"" << html_string_escape(value) << "\"";
}

void encoder::encode(std::ostream &out, const binary &value)
{
    out << "&" << base64_encode(value);
}

} // namespace decof
