/*
 * Copyright (c) 2019 Florian Behrens
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

#include "basic_parameter.h"
#include "encoding_hint.h"
#include "typed_client_read_interface.h"

namespace decof {

template <typename T, encoding_hint EncodingHint = encoding_hint::none>
class readable_parameter : public basic_parameter<T, EncodingHint>, public typed_client_read_interface<T, EncodingHint>
{
  protected:
    using basic_parameter<T, EncodingHint>::basic_parameter;
};

} // namespace decof

#endif // DECOF_READABLE_PARAMETER_H
