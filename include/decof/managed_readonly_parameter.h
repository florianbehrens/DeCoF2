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

#include "readable_parameter.h"

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_MANAGED_READONLY_PARAMETER(type_name, value_type)       \
    struct type_name : public decof::managed_readonly_parameter<value_type> { \
        type_name(std::string name, decof::node *parent, decof::userlevel_t readlevel = decof::Normal, const value_type &value = value_type()) : \
            decof::managed_readonly_parameter<value_type>(name, parent, readlevel, value) {} \
    }

namespace decof
{

/**
 * A managed_readonly_parameter may only by modified by the server side.
 *
 * This parameter type can be monitored efficiently.
 */
template<typename T>
class managed_readonly_parameter : public readable_parameter<T>
{
public:
    managed_readonly_parameter(std::string name, node *parent, const T &value)
     : readable_parameter<T>(name, parent, Normal, Forbidden),
       value_(value)
    {}

    managed_readonly_parameter(std::string name, node *parent, userlevel_t readlevel = Normal, const T &value = T())
     : readable_parameter<T>(name, parent, readlevel, Forbidden),
       value_(value)
    {}

    virtual T value() override final {
        return value_;
    }

    void set_value(const T &value)
    {
        if (value_ != value) {
            value_ = value;
            readable_parameter<T>::signal(value);
        }
    }

private:
    T value_;
};

} // namespace decof

#endif // MANAGED_READONLY_PARAMETER_H
