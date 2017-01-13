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

#ifndef EXTERNAL_READWRITE_PARAMETER_H
#define EXTERNAL_READWRITE_PARAMETER_H

#include <string>

#include "readable_parameter.h"
#include "typed_client_write_interface.h"

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_EXTERNAL_READWRITE_PARAMETER(type_name, value_type)     \
    struct type_name : public decof::external_readwrite_parameter<value_type> { \
        type_name(std::string name, decof::node *parent, decof::userlevel_t readlevel = decof::Readonly, decof::userlevel_t writelevel = decof::Normal) : \
            decof::external_readwrite_parameter<value_type>(name, parent, readlevel, writelevel) {} \
        private:                                                              \
            virtual bool external_value(const value_type &value) override;\
            virtual value_type external_value() override;                 \
    }

namespace decof
{

/**
 * An external_readwrite_parameter may only be modified by the client side and is
 * managed by the server implementation, i.e. externally from the framework's
 * viewpoint.
 *
 * This parameter type can be monitored efficiently.
 */
template<typename T>
class external_readwrite_parameter : public readable_parameter<T>, public typed_client_write_interface<T>
{
public:
    external_readwrite_parameter(std::string name, node *parent, userlevel_t readlevel = Normal, userlevel_t writelevel = Normal) :
        readable_parameter<T>(name, parent, readlevel, writelevel)
    {}

    virtual T value() const override final {
        return external_value();
    }

private:
    virtual void value(const T &value) override final {
        if (external_value(value) == true)
            readable_parameter<T>::emit(value);
    }

    virtual bool external_value(const T &value) = 0;
    virtual T external_value() const = 0;
};

} // namespace decof

#endif // EXTERNAL_READWRITE_PARAMETER_H
