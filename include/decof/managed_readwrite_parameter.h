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

#ifndef MANAGED_READWRITE_PARAMETER_H
#define MANAGED_READWRITE_PARAMETER_H

#include <string>
#include <vector>

#include "observable_parameter.h"
#include "readwrite_parameter.h"

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_MANAGED_READWRITE_PARAMETER(type_name, value_type)      \
    struct type_name : public decof::managed_readwrite_parameter<value_type> { \
        type_name(std::string name, decof::node *parent, decof::userlevel_t readlevel = decof::Readonly, decof::userlevel_t writelevel = decof::Normal, const value_type &value = value_type()) : \
            decof::managed_readwrite_parameter<value_type>(name, parent, readlevel, writelevel, value) {} \
        type_name(std::string name, decof::node *parent, const value_type &value) : \
            decof::managed_readwrite_parameter<value_type>(name, parent, decof::Readonly, decof::Normal, value) {} \
        virtual void verify(const value_type& value) override;                \
    }

namespace decof
{

// Forward declaration
template<typename T>
class managed_readonly_parameter;

/**
 * A managed_readwrite_parameter may only be modified by the client side.
 *
 * This parameter type can be monitored efficiently.
 *
 * T requirements:
 * - Defaultconstructible
 * - Copyconstructible
 */
template<typename T>
class managed_readwrite_parameter : public observable_parameter<T>, public readwrite_parameter<T>
{
    friend class managed_readonly_parameter<T>;

public:
    managed_readwrite_parameter(std::string name, node *parent, const T &value)
     : observable_parameter<T>(name, parent, Readonly, Normal), value_(value)
    {}

    managed_readwrite_parameter(std::string name, node *parent, userlevel_t readlevel = Readonly, userlevel_t writelevel = Normal, const T &value = T())
     : observable_parameter<T>(name, parent, readlevel, writelevel), value_(value)
    {}

    virtual T value() {
        return value_;
    }

protected:
    virtual void verify(const T&)
    {}

private:
    virtual void set_private_value(const T &value) final
    {
        if (value_ == value)
            return;

        verify(value);
        value_ = value;
        observable_parameter<T>::signal(value);
    }

    T value_;
};

} // namespace decof

#endif // MANAGED_READWRITE_PARAMETER_H