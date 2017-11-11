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

#ifndef DECOF_CLIENT_WRITE_INTERFACE_H
#define DECOF_CLIENT_WRITE_INTERFACE_H

#include "types.h"

namespace decof {

/** @brief Interface for client write access to parameter values.
 *
 * Since this interface's only purpose is to be used by an object of type
 * #client_context, the latter is declared as friend and all members are
 * private. */
class client_write_interface
{
    friend class client_context;

public:
    virtual ~client_write_interface() {}

private:
    /** @brief Set value wrapped in a runtime dynamic type.
     *
     * TODO
     *
     * Scalar and sequence types must be wrapped in a generic_value as they
     * are. Tuple types must be dismantled and the individual elements wrapped
     * in a std::vector of generic_values which is, in turn, again wrapped
     * in a generic_value. */
    virtual void value(const generic_value& any_value) = 0;
};

} // namespace decof

#endif // DECOF_CLIENT_WRITE_INTERFACE_H
