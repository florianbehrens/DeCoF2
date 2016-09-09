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

#include "basic_parameter.h"
#include "conversion.h"
#include "object_visitor.h"
#include "typed_client_read_interface.h"

namespace decof
{

template<typename T>
class readable_parameter : public basic_parameter<T>, public typed_client_read_interface<T>
{
public:
    virtual boost::signals2::scoped_connection observe(client_read_interface::slot_type slot) override {
        boost::signals2::scoped_connection retval = signal_.connect(slot);
        signal(this->value());
        return retval;
    }

    /// Visitor pattern accept method
    virtual void accept(object_visitor *visitor) override {
        visitor->visit(this);
    }

protected:
    // We inherit base class constructors
    using basic_parameter<T>::basic_parameter;

    void signal(const T& value) {
        signal_(this->fq_name(), boost::any(value));
    }

private:
    client_read_interface::signal_type signal_;
};

} // namespace decof

#endif // DECOF_READABLE_PARAMETER_H
