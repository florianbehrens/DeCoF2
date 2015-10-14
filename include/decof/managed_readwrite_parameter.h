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

#ifndef DECOF_MANAGED_READWRITE_PARAMETER_H
#define DECOF_MANAGED_READWRITE_PARAMETER_H

#include <string>

#include "readable_parameter.h"
#include "typed_client_write_interface.h"

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_MANAGED_READWRITE_PARAMETER(type_name, value_type)      \
    struct type_name : public decof::managed_readwrite_parameter<value_type> { \
        type_name(std::string name, decof::node *parent, decof::userlevel_t readlevel = decof::Normal, decof::userlevel_t writelevel = decof::Normal, const value_type &value = value_type()) : \
            decof::managed_readwrite_parameter<value_type>(name, parent, readlevel, writelevel, value) {} \
        type_name(std::string name, decof::node *parent, const value_type &value) : \
            decof::managed_readwrite_parameter<value_type>(name, parent, decof::Normal, decof::Normal, value) {} \
        virtual void verify(const value_type& value) override;                \
    }

namespace decof
{

/**
 * A managed_readwrite_parameter may only be modified by the client side.
 *
 * This parameter type can be monitored efficiently.
 */
template<typename T>
class managed_readwrite_parameter : public readable_parameter<T>, public typed_client_write_interface<T>
{
public:
    managed_readwrite_parameter(const std::string &name, node *parent, const T &value) :
        readable_parameter<T>(name, parent, Normal, Normal), value_(value)
    {}

    managed_readwrite_parameter(const std::string &name, node *parent,
                                userlevel_t readlevel = Normal, userlevel_t writelevel = Normal,
                                const T &value = T()) :
        readable_parameter<T>(name, parent, readlevel, writelevel), value_(value)
    {}

    virtual T value() override final {
        return value_;
    }

protected:
    virtual void verify(const T&)
    {}

private:
    virtual void value(const T &value) override final
    {
        if (value_ == value)
            return;

        verify(value);
        value_ = value;
        readable_parameter<T>::signal(value);
    }

    T value_;
};

} // namespace decof

#endif // DECOF_MANAGED_READWRITE_PARAMETER_H
