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

#ifndef DECOF_BASIC_PARAMETER_H
#define DECOF_BASIC_PARAMETER_H

#include "conversion.h"
#include "encoding_hint.h"
#include "object.h"
#include "object_visitor.h"

namespace decof {

/** @brief Basic parameter type.
 *
 * @tparam T The parameter value type.
 *
 * A basic_parameter adds to the #object class the concept of a parameter
 * value of a certain type. It does not contain any accessing functions.
 */
template <typename T, encoding_hint EncodingHint>
class basic_parameter : public object
{
  public:
    typedef T value_type;

    virtual void accept(object_visitor* visitor) override
    {
        visitor->visit(this, typename conversion_helper<T, EncodingHint>::type_tag());
    }

  protected:
    // We inherit base class constructors
    using object::object;
};

} // namespace decof

#endif // DECOF_BASIC_PARAMETER_H
