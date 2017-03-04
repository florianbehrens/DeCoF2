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

namespace decof
{

enum error_codes {
    SUCCESS = 0,
    UNKNOWN_ERROR,
    PARSE_ERROR,
    BAD_REQUEST,
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

/**
 * @brief Parse error exception.
 *
 * A parse error is thrown when the request cannot be parsed at all.
 */
struct parse_error : public runtime_error
{
    parse_error();
};

/**
 * @brief Bad request error exception.
 *
 * A bad request error is thrown when the request can be parsed but is not
 * consistent or errorneous.
 */
struct bad_request_error : public runtime_error
{
    bad_request_error();
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

/**
 * @brief Unknown operation error exception.
 *
 * A unknown operation error is thrown when a legal request contains an unknown
 * or undefined operation.
 */
struct unknown_operation_error : public runtime_error
{
    unknown_operation_error();
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
