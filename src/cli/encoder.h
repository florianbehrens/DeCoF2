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

#ifndef DECOF_CLI_ENCODER_H
#define DECOF_CLI_ENCODER_H

#include <string>

#include "basic_value_encoder.h"

namespace decof
{

namespace cli
{

struct encoder : public basic_value_encoder
{
    virtual void encode(std::ostream &out, const boolean &value);
    virtual void encode(std::ostream &out, const string &value);
    virtual void encode(std::ostream &out, const binary &value);
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_ENCODER_H
