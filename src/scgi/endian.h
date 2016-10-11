/*
 * Copyright (c) 2015 Florian Behrens
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

#ifndef DECOF_SCGI_ENDIAN_H
#define DECOF_SCGI_ENDIAN_H

#include <cstdint>
#include <cstddef>

namespace decof
{

namespace scgi
{

template<typename T>
T little_endian_to_native(const T &value)
{
    T retval = value;

    // System is big endian?
    uint32_t test = 1;
    if (*reinterpret_cast<const char*>(&test) != '\001') {
        const size_t size = sizeof(T);
        const char *input = reinterpret_cast<const char *>(&value);
        char *output = reinterpret_cast<char *>(&retval);
        for (size_t i = 0; i < size; ++i)
            output[i] = input[size-1-i];
    }

    return retval;
}

template<typename T>
T native_to_little_endian(const T &value)
{
    T retval = value;

    // System is big endian?
    uint32_t test = 1;
    if (*reinterpret_cast<const char*>(&test) != '\001') {
        const size_t size = sizeof(T);
        const char *input = reinterpret_cast<const char *>(&value);
        char *output = reinterpret_cast<char *>(&retval);
        for (size_t i = 0; i < size; ++i)
            output[i] = input[size-1-i];
    }

    return retval;
}

} // namespace decof

} // namespace scgi

#endif // DECOF_SCGI_ENDIAN_H
