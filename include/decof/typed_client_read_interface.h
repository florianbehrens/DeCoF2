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
#include "encoding_hint.h"

namespace decof {

template <typename T, encoding_hint EncodingHint = encoding_hint::none>
class typed_client_read_interface : public client_read_interface
{
  public:
    /// Parameter value getter function.
    virtual T value() const = 0;

    virtual value_t generic_value() const override final
    {
        return conversion_helper<T, EncodingHint>::to_generic(value());
    }
};

} // namespace decof

#endif // DECOF_TYPED_CLIENT_READ_INTERFACE_H
