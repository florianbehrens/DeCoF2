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

#ifndef DECOF_SCGI_JS_VALUE_ENCODER_H
#define DECOF_SCGI_JS_VALUE_ENCODER_H

#include <decof/client_context/basic_value_encoder.h>

namespace decof
{

namespace scgi
{

struct js_value_encoder : public decof::basic_value_encoder
{
    void encode_boolean(std::ostream &out, const boolean &value) override;
    void encode_boolean_seq(std::ostream &out, const boolean_seq &value) override;
    void encode_integer_seq(std::ostream &out, const integer_seq &value) override;
    void encode_real_seq(std::ostream &out, const real_seq &value) override;
    void encode_string_seq(std::ostream &out, const string_seq &value) override;
    void encode_binary_seq(std::ostream &out, const binary_seq &value) override;
    void encode_tuple(std::ostream &out, const dynamic_tuple &value) override;
};

} // namespace scgi

} // namespace decof

#endif // DECOF_SCGI_JS_VALUE_ENCODER_H
