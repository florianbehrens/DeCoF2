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

#ifndef MANAGED_READONLY_PARAMETER_H
#define MANAGED_READONLY_PARAMETER_H

#include <string>
#include <vector>

#include "managed_readwrite_parameter.h"

/**
 * A managed_readonly_parameter may only by modified by the server side.
 *
 * This parameter type can be monitored efficiently.
 */
template<typename T>
class managed_readonly_parameter : public managed_readwrite_parameter<T>
{
public:
    typedef T value_type;

    // We inherit base class constructors
    using managed_readwrite_parameter<T>::managed_readwrite_parameter;

    void set_value(const value_type &value)
    {
        if (managed_readwrite_parameter<T>::value_ != value) {
            managed_readwrite_parameter<T>::value_ = value;
            observable_parameter<T>::signal(value);
        }
    }
};

#endif // MANAGED_READONLY_PARAMETER_H
