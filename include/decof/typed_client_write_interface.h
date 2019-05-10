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

#ifndef DECOF_TYPED_CLIENT_WRITE_INTERFACE_H
#define DECOF_TYPED_CLIENT_WRITE_INTERFACE_H

#include "client_write_interface.h"
#include "encoding_hint.h"
#include "exceptions.h"

namespace decof {

template <typename T, encoding_hint EncodingHint = encoding_hint::none>
class typed_client_write_interface : public client_write_interface
{
  private:
    /// Parameter value setter function.
    virtual void value(const T&) = 0;

    virtual void generic_value(const value_t& value) override final
    {
        this->value(conversion_helper<T, EncodingHint>::from_generic(value));
    }
};

} // namespace decof

#endif // DECOF_TYPED_CLIENT_WRITE_INTERFACE_H
