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

#ifndef DECOF_OBSERVABLE_PARAMETER_H
#define DECOF_OBSERVABLE_PARAMETER_H

#include "basic_parameter.h"
#include "client_observe_interface.h"
#include "conversion.h"
#include "encoding_hint.h"
#include "object_visitor.h"
#include "typed_client_read_interface.h"
#include <boost/signals2/connection.hpp>

namespace decof {

template <typename T, encoding_hint EncodingHint = encoding_hint::none>
class observable_parameter : public basic_parameter<T, EncodingHint>,
                             public typed_client_read_interface<T, EncodingHint>,
                             public client_observe_interface
{
  public:
    virtual boost::signals2::scoped_connection observe(value_change_slot slot) override
    {
        boost::signals2::scoped_connection retval = signal_.connect(slot);
        emit(this->value());
        return retval;
    }

    /**
     * @note The implementation of this function can check whether there are
     * still other client contexts connected by calling @code
     * signal().num_slots() @endcode.
     */
    virtual void unobserve() override
    {
    }

  protected:
    // We inherit base class constructors
    using basic_parameter<T, EncodingHint>::basic_parameter;

    /** @brief Emit parameter value observation signal.
     *
     * @param value The value to be reported to the connected slot(s).
     */
    void emit(const T& value)
    {
        signal_(this->fq_name(), conversion_helper<T, EncodingHint>::to_generic(value));
    }

    value_change_signal signal_;
};

} // namespace decof

#endif // DECOF_OBSERVABLE_PARAMETER_H
