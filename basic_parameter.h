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

#ifndef BASIC_PARAMETER_H
#define BASIC_PARAMETER_H

#include <vector>
#include <string>

#include <boost/any.hpp>

#include "container_types.h"
#include "tree_element.h"

template<typename T>
class basic_parameter : public tree_element
{
public:
    typedef T value_type;

    // This method is not const because external parameters might need to
    // alter state, e.g., when reading the value from a file.
    // Another possible solution could be to make those state holding members
    // mutable.
    virtual value_type value() = 0;

    virtual boost::any any_value() noexcept override {
        return boost::any(value());
    }

protected:
    // We inherit base class constructors
    using tree_element::tree_element;
};

#endif // BASIC_PARAMETER_H
