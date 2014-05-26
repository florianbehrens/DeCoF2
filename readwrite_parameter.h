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

#ifndef READWRITE_PARAMETER_H
#define READWRITE_PARAMETER_H

template<typename T>
class readwrite_parameter
{
    friend class ClientProxy;

public:
    typedef T value_type;

private:
   virtual void set_private_value(const value_type &value) = 0;
};

#endif // READWRITE_PARAMETER_H
