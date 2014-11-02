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

#include "object_dictionary.h"
#include "typed_parameter.h"

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(type_name, value_type)      \
    struct type_name : public decof::external_readonly_parameter<value_type> { \
        type_name(std::string name, decof::node *parent, decof::userlevel_t readlevel = decof::Readonly) : \
            decof::external_readonly_parameter<value_type>(name, parent, readlevel) {} \
        virtual value_type get_external_value() override;                     \
    }

namespace decof
{

/**
 * An external_readonly_parameter may only be modified by the server side and is
 * managed by the server implementation, i.e. externally from the framework's
 * viewpoint.
 *
 * An external_readonly_parameter is the only parameter type that internally
 * uses a polling timer for monitoring.
 */
template<typename T>
class external_readonly_parameter : public typed_parameter<T>
{
public:
    external_readonly_parameter(std::string name, node *parent, userlevel_t readlevel = Readonly) :
        typed_parameter<T>(name, parent, readlevel, Infinite)
    {}

    virtual ~external_readonly_parameter() {
        connection_.disconnect();
    }

    virtual T value() override final {
        return get_external_value();
    }

    virtual boost::signals2::connection observe(object::slot_type slot) override {
        // Check for object dictionary
        object_dictionary* obj_dict = this->get_object_dictionary();
        if (obj_dict == nullptr)
            return boost::signals2::connection();

        // Connect to regular timer
        connection_ = obj_dict->get_medium_timer().observe(std::bind(&external_readonly_parameter<T>::notify, this));

        // Call base class member function
        return typed_parameter<T>::observe(slot);
    }

private:
    virtual T get_external_value() = 0;

    /// Slot member function for @a regular_timer.
    void notify() {
        T cur_value = value();
        if (last_value_ != cur_value) {
            typed_parameter<T>::signal(cur_value);
            last_value_ = cur_value;
        }
    }

    boost::signals2::connection connection_;
    T last_value_;
};

} // namespace decof

#endif // EXTERNAL_READONLY_PARAMETER_H
