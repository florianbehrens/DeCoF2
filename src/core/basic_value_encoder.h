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

#ifndef DECOF_BASIC_VALUE_ENCODER_H
#define DECOF_BASIC_VALUE_ENCODER_H

#include <string>
#include <ostream>

#include <boost/any.hpp>

#include "conversion.h"
#include "types.h"

namespace decof
{

/** General-pupose value encoder.
 *
 * This class provides a standard iostreams-based encoding for all types in the
 * DeCoF type system. Client contexts may choose to use the provided encodings
 * or to overload them.
 */
struct basic_value_encoder
{
    virtual ~basic_value_encoder();

    /// HTML string escape function.
    /// Performs HTML string escaping for non-printable characters
    /// identified by std::isprint() and '"'.
    /// @param str A const reference to the string to be escaped.
    /// @returns The escaped string.
    static std::string html_string_escape(const std::string &str);

    /// Base64 string encoder function.
    static std::string base64_encode(const decof::binary &bin);

    /// Encoding entry function overload.
    /// Calls the type-specific encoding functions.
    void encode_any(std::string &str, const boost::any &any_value);

    /// Encoding entry function overload.
    /// Calls the type-specific encoding functions.
    void encode_any(std::ostream &out, const boost::any &any_value);

    virtual void encode(std::ostream &out, const boolean &value);
    virtual void encode(std::ostream &out, const integer &value);
    virtual void encode(std::ostream &out, const real &value);
    virtual void encode(std::ostream &out, const string &value);
    virtual void encode(std::ostream &out, const binary &value);

    virtual void encode(std::ostream &out, const boolean_seq &value);
    virtual void encode(std::ostream &out, const integer_seq &value);
    virtual void encode(std::ostream &out, const real_seq &value);
    virtual void encode(std::ostream &out, const string_seq &value);
    virtual void encode(std::ostream &out, const binary_seq &value);

    virtual void encode(std::ostream &out, const dynamic_tuple &value);
};

} // namespace decof

#endif // DECOF_BASIC_VALUE_ENCODER_H
