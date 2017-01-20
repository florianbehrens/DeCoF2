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

#ifndef DECOF_READABLE_PARAMETER_H
#define DECOF_READABLE_PARAMETER_H

#include <boost/signals2/connection.hpp>

#include <decof/client_context/object_visitor.h>

#include "basic_parameter.h"
#include "conversion.h"
#include "typed_client_read_interface.h"

namespace decof
{

template<typename T>
class readable_parameter : public basic_parameter<T>, public typed_client_read_interface<T>
{
public:
    /// Override client_read_interface::observe.
    virtual boost::signals2::scoped_connection observe(client_read_interface::value_change_slot_t slot) override {
        boost::signals2::scoped_connection retval = signal_.connect(slot);
        emit(this->value());
        return retval;
    }

    /** @brief  Override client_read_interface::unobserve.
     *
     * @note The implementation of this function can check whether there are
     * still other client contexts connected by calling @code
     * signal().num_slots() @endcode.
     */
    virtual void unobserve() override {
    }

    /// Visitor pattern accept method
    virtual void accept(object_visitor *visitor) override {
        visitor->visit(this);
    }

protected:
    // We inherit base class constructors
    using basic_parameter<T>::basic_parameter;

    /** @brief Emit parameter value observation signal.
     *
     * @param value The value to be reported to the connected slot(s).
     */
    void emit(const T& value) {
        signal_(this->fq_name(), boost::any(value));
    }

    client_read_interface::value_change_signal_t signal_;
};

} // namespace decof

#endif // DECOF_READABLE_PARAMETER_H
