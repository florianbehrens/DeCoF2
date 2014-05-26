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

#include "observable_parameter.h"
#include "readwrite_parameter.h"

/**
 * An external_readwrite_parameter may only be modified by the client side and is
 * managed by the server implementation, i.e. externally from the framework's
 * viewpoint.
 *
 * This parameter type can be monitored efficiently.
 */
template<typename T>
class external_readwrite_parameter : public observable_parameter<T>, public readwrite_parameter<T>
{
public:
    typedef T value_type;

    external_readwrite_parameter(std::string name, node *parent = nullptr)
     : observable_parameter<T>(name, parent)
    {}

    virtual value_type value() {
        return get_external_value();
    }

private:
    virtual void set_private_value(const value_type &value) final {
        if (set_external_value(value) == true)
            observable_parameter<T>::signal(value);
    }

    virtual bool set_external_value(const value_type &value) = 0;
    virtual value_type get_external_value() = 0;
};

#endif // EXTERNAL_READWRITE_PARAMETER_H
