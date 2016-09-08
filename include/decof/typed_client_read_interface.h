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

#ifndef DECOF_TYPED_CLIENT_READ_INTERFACE_H
#define DECOF_TYPED_CLIENT_READ_INTERFACE_H

#include "client_read_interface.h"
#include "conversion.h"

namespace decof
{

template<typename T>
class typed_client_read_interface : public client_read_interface
{
public:
    virtual T value() const = 0;

    /// @brief Return value as runtime dynamic type.
    /// Scalar and sequence types are wrapped in a boost::any as they are.
    /// Tuple types are dismantled and the individual elements wrapped in a
    /// vector of boost::anys which is, in turn, again wrapped in a boost::any.
    virtual boost::any any_value() override final {
        return Conversion<T>::to_any(value());
    }
};

} // namespace decof

#endif // DECOF_TYPED_CLIENT_READ_INTERFACE_H
