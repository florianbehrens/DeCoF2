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

#ifndef DECOF_JSON_VALUE_ENCODER_H
#define DECOF_JSON_VALUE_ENCODER_H

#include <decof/client_context/basic_value_encoder.h>

namespace decof
{

/**
 * @brief JSON value encoder.
 *
 * See http://www.json.org for more information on JSON value encoding.
 *
 * @note Binary parameter value types are not supported.
 */
struct json_value_encoder : public decof::basic_value_encoder
{
    virtual void encode_string(std::ostream& out, const string& value) override;
    virtual void encode_binary(std::ostream& out, const binary& value) override;
    virtual void encode_tuple(std::ostream& out, const dynamic_tuple& value) override;
};

} // namespace decof

#endif // DECOF_JSON_VALUE_ENCODER_H
