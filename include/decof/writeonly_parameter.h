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

#ifndef DECOF_WRITEONLY_PARAMETER_H
#define DECOF_WRITEONLY_PARAMETER_H

#include <decof/client_context/object_visitor.h>

#include "basic_parameter.h"
#include "typed_client_write_interface.h"

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_WRITEONLY_PARAMETER(type_name, value_type)               \
    struct type_name : public decof::writeonly_parameter<value_type> {         \
        type_name(std::string name, decof::node *parent, decof::userlevel_t writelevel = decof::Normal) : \
            decof::writeonly_parameter<value_type>(name, parent, writelevel) {} \
        virtual void value(const value_type &value) override;                   \
    }

namespace decof
{

template<typename T>
class writeonly_parameter : public basic_parameter<T>, public typed_client_write_interface<T>
{
public:
    writeonly_parameter(std::string name, node *parent, userlevel_t writelevel = Normal) :
        basic_parameter<T>(name, parent, Forbidden, writelevel)
    {}

    /// Visitor pattern accept method
    virtual void accept(object_visitor *visitor) override {
        visitor->visit(this);
    }
};

} // namespace decof

#endif // DECOF_WRITEONLY_PARAMETER_H
