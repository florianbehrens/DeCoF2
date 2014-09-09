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

#ifndef OBSERVABLE_PARAMETER_H
#define OBSERVABLE_PARAMETER_H

#include "basic_parameter.h"

namespace decof
{

template<typename T>
class observable_parameter : public basic_parameter
{
public:
    typedef T value_type;

    // This method is not const because external parameters might need to
    // alter state, e.g., when reading the value from a file.
    // Another possible solution could be to make those state holding members
    // mutable.
    virtual value_type value() = 0;

    virtual boost::any any_value() override final {
        return boost::any(value());
    }

    virtual boost::signals2::connection observe(tree_element::slot_type slot) noexcept override {
        boost::signals2::connection retval = signal_.connect(slot);
        signal(this->value());
        return retval;
    }

protected:
    // We inherit base class constructors
    using basic_parameter::basic_parameter;

    void signal(const value_type& value) {
        signal_(this->fq_name(), boost::any(value));
    }

private:
    tree_element::signal_type signal_;
};

} // namespace decof

#endif // OBSERVABLE_PARAMETER_H
