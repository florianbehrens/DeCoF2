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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

struct access_denied_error : public std::runtime_error
{
    access_denied_error();
};

struct invalid_parameter_error : public std::runtime_error
{
    invalid_parameter_error();
};

struct wrong_type_error : public std::runtime_error
{
    wrong_type_error();
};

struct invalid_value_error : public std::runtime_error
{
    invalid_value_error();
};

#endif // EXCEPTIONS_H
