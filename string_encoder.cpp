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

#include "string_encoder.h"

#include <boost/any.hpp>
#include <boost/algorithm/string/join.hpp>

#include "container_types.h"
#include "exceptions.h"

std::string string_encoder::encode(const boost::any &any_value)
{
    if (any_value.type() == typeid(std::string))
        return std::string("\"") + boost::any_cast<std::string>(any_value) + "\"";
    else if (any_value.type() == typeid(stringlist))
        return std::string("[\"") + boost::algorithm::join(boost::any_cast<stringlist>(any_value), std::string("\",\"")) + "\"]";
    else
        throw wrong_type_error();
}
