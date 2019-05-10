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

namespace decof {

enum error_codes {
    SUCCESS = 0,
    UNKNOWN_ERROR,
    PARSE_ERROR,
    ACCESS_DENIED,
    INVALID_PARAMETER,
    WRONG_TYPE,
    INVALID_VALUE,
    UNKNOWN_OPERATION,
    INVALID_USERLEVEL,
    NOT_SUBSCRIBED,
    NOT_IMPLEMENTED,
    PROTOCOL_SPECIFIC_ERRORS
};

class runtime_error : public std::runtime_error
{
  protected:
    explicit runtime_error(int code, const std::string& what);

  public:
    int code();

  private:
    int code_;
};

struct access_denied_error : public runtime_error
{
    access_denied_error();
};

struct invalid_parameter_error : public runtime_error
{
    invalid_parameter_error();
};

struct wrong_type_error : public runtime_error
{
    wrong_type_error();
};

struct invalid_value_error : public runtime_error
{
    invalid_value_error();
};

struct unknown_operation_error : public runtime_error
{
    unknown_operation_error();
};

struct parse_error : public runtime_error
{
    parse_error();
};

struct invalid_userlevel_error : public runtime_error
{
    invalid_userlevel_error();
};

struct not_subscribed_error : public runtime_error
{
    not_subscribed_error();
};

struct not_implemented_error : public runtime_error
{
    not_implemented_error();
};

} // namespace decof

#endif // EXCEPTIONS_H
