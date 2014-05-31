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

#ifndef EXTERNAL_READONLY_PARAMETER_H
#define EXTERNAL_READONLY_PARAMETER_H

#include <string>

#include "basic_parameter.h"

namespace decof
{

class observer;

/**
 * An external_readonly_parameter may only be modified by the server side and is
 * managed by the server implementation, i.e. externally from the framework's
 * viewpoint.
 *
 * An external_readonly_parameter is the only parameter type that cannot be
 * monitored efficiently. Rather, it must be polled.
 */
template<typename T>
class external_readonly_parameter : public basic_parameter<T>
{
public:
    typedef T value_type;

    // We inherit base class constructors
    using basic_parameter<T>::basic_parameter;

    virtual value_type value() final {
        return get_external_value();
    }

    virtual tree_element::connection observe(tree_element::slot_type) noexcept override {
        std::cerr << this->fq_name() << " cannot yet be observed!" << std::endl;
        return tree_element::connection();
    }

private:
    virtual value_type get_external_value() = 0;
};

} // namespace decof

#endif // EXTERNAL_READONLY_PARAMETER_H
