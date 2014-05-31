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

#ifndef ERRORS_H
#define ERRORS_H

namespace decof
{

enum errors {
    SUCCESS = 0,
    UNKNOWN_ERROR,
    ACCESS_DENIED_ERROR,
    INVALID_PARAMETER_ERROR,
    WRONG_TYPE_ERROR,
    INVALID_VALUE_ERROR,
    UNKNOWN_OPERATION_ERROR,
    PROTOCOL_SPECIFIC_ERRORS
};

} // namespace decof

#endif // ERRORS_H
