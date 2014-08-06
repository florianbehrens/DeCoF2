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
#include "observable_parameter.h"

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
class external_readonly_parameter : public observable_parameter<T>
{
public:
    typedef T value_type;

    // We inherit base class constructors
    using observable_parameter<T>::observable_parameter;

    virtual ~external_readonly_parameter() {
        connection_.disconnect();
    }

    virtual value_type value() final {
        return get_external_value();
    }

    virtual tree_element::connection observe(tree_element::slot_type slot) noexcept override {
        // Check for object dictionary
        object_dictionary* obj_dict = this->get_object_dictionary();
        if (obj_dict == nullptr)
            return tree_element::connection();

        // Connect to regular timer
        connection_ = obj_dict->get_medium_timer().observe(std::bind(&external_readonly_parameter<T>::notify, this));

        // Call base class member function
        return observable_parameter<T>::observe(slot);
    }

private:
    virtual value_type get_external_value() = 0;

    /// Slot member function for @a regular_timer.
    void notify() {
        value_type cur_value = value();
        if (last_value_ != cur_value) {
            observable_parameter<T>::signal(cur_value);
            last_value_ = cur_value;
        }
    }

    boost::signals2::connection connection_;
    value_type last_value_;
};

} // namespace decof

#endif // EXTERNAL_READONLY_PARAMETER_H
