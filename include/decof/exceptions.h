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

/**
 * @brief The base class for all DeCoF specific errors.
 *
 * This class is designed to be used as base class for all DeCoF specific
 * exception classes. It cannot be instantiated as its constructor is
 * protected.
 */
class decof_error : public std::runtime_error
{
protected:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Parse error exception.
 *
 * A parse error is thrown when the request cannot be parsed at all.
 */
struct parse_error : public decof_error
{
    parse_error();
};

/**
 * @brief Bad request error exception.
 *
 * A bad request error is thrown when the request can be parsed but is not
 * consistent or errorneous.
 */
struct bad_request_error : public decof_error
{
    bad_request_error();
};

/**
 * @brief Access denied error exception.
 *
 * An access denied error is thrown when a client tries to modify a readonly
 * parameter or to read a writeonly parameter. Additionally, this error occurs
 * when the client's userlevel is not sufficient for the desired operation.
 */
struct access_denied_error : public decof_error
{
    access_denied_error();
};

/**
 * @brief Invalid parameter path error exception.
 *
 * This exception is thrown in case of an invalid/not available parameter path
 * given.
 */
struct invalid_parameter_error : public decof_error
{
    invalid_parameter_error();
};

/**
 * @brief Wrong parameter value type error exception.
 *
 * This exception is thrown in case the given parameter value type does not
 * match.
 */
struct wrong_type_error : public decof_error
{
    wrong_type_error();
};

/**
 * @brief Invalid parameter value error exception.
 *
 * An invalid value error is thrown when a parameter value has a matching type
 * but its value is illegal, i.e., it exceeds any bounds or is not accepted
 * otherwise.
 */
struct invalid_value_error : public decof_error
{
    invalid_value_error();
};

/**
 * @brief Unknown operation error exception.
 *
 * A unknown operation error is thrown when a legal request contains an unknown
 * or undefined operation.
 */
struct unknown_operation_error : public decof_error
{
    unknown_operation_error();
};

/**
 * @brief Invalid userlevel error exception.
 *
 * This exception is thrown on attempts to change the userlevel if the desired
 * userlevel is invalid.
 */
struct invalid_userlevel_error : public decof_error
{
    invalid_userlevel_error();
};

/**
 * @brief Not subscribed error exception.
 *
 * This exception is thrown on unsubscribe operations on parameters that have
 * not been subscribed before.
 */
struct not_subscribed_error : public decof_error
{
    not_subscribed_error();
};

/**
 * @brief Not implemented error exception.
 *
 * The not implemented error exception is thrown for yet not implemented
 * features.
 */
struct not_implemented_error : public decof_error
{
    not_implemented_error();
};

} // namespace decof

#endif // EXCEPTIONS_H
