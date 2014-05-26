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

#ifndef SCHEME_ERRORS_H
#define SCHEME_ERRORS_H

enum scheme_errors {
    SCHEME_NO_ERROR = 0,
    SCHEME_UNKNOWN_ERROR,
    SCHEME_ACCESS_DENIED_ERROR,
    SCHEME_INVALID_PARAMETER_ERROR,
    SCHEME_WRONG_TYPE_ERROR,
    SCHEME_PARSE_ERROR,
    SCHEME_INVALID_VALUE_ERROR,
    SCHEME_UNKNOWN_OPERATION_ERROR
};

#endif // SCHEME_ERRORS_H
