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

#ifndef BASIC_READWRITE_PARAMETER_H
#define BASIC_READWRITE_PARAMETER_H

#include <boost/any.hpp>

namespace decof {

class basic_readwrite_parameter
{
    friend class client_context;

private:
    virtual void set_private_value(const boost::any& any_value) = 0;
};

} // namespace decof

#endif // BASIC_READWRITE_PARAMETER_H
