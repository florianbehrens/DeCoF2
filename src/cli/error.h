/*
 * Copyright (c) 2017 Florian Behrens
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

#ifndef DECOF_CLI_ERROR_H
#define DECOF_CLI_ERROR_H

#include <decof/exceptions.h>

namespace decof {
namespace cli {

enum error {
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
    NOT_IMPLEMENTED
};

size_t error_code_from_exception(const decof_error& ex);

} } // namespace cli, decof

#endif // DECOF_CLI_ERROR_H
