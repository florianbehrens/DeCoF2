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

#ifndef DECOF_TYPED_PARAMETER_H
#define DECOF_TYPED_PARAMETER_H

#include "basic_parameter.h"
#include "conversion.h"
#include "object_visitor.h"

namespace decof
{

template<typename T>
class typed_parameter : public basic_parameter
{
public:
    typedef T value_type;

    /// @note This method is not const because external parameters might need to
    /// alter state, e.g., when reading the value from a file.
    /// Another possible solution could be to make those state holding members
    /// mutable.
    virtual T value() = 0;

    /// @brief Return value as runtime dynamic type.
    /// Scalar and sequence types are wrapped in a boost::any as they are.
    /// Tuple types are dismantled and the individual elements wrapped in a
    /// vector of boost::anys which is, in turn, again wrapped in a boost::any.
    virtual boost::any any_value() override final {
        return Conversion<T>::to_any(value());
    }

    virtual boost::signals2::connection observe(object::slot_type slot) override {
        boost::signals2::connection retval = signal_.connect(slot);
        signal(this->value());
        return retval;
    }

    /// Visitor pattern accept method
    virtual void accept(object_visitor *visitor) override {
        visitor->visit(this);
    }

protected:
    // We inherit base class constructors
    using basic_parameter::basic_parameter;

    void signal(const T& value) {
        signal_(this->fq_name(), boost::any(value));
    }

private:
    object::signal_type signal_;
};

} // namespace decof

#endif // DECOF_TYPED_PARAMETER_H
