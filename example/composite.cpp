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

#include "composite.h"

composite::composite(std::string name, decof::node *parent) :
    node(name, parent),
    summand1_("summand1", this),
    summand2_("summand2", this),
    sum_("sum", this)
{
    summand1_.verify_handler(std::bind(&composite::summand1_changing, this,
                                       std::placeholders::_1));
    summand2_.verify_handler(std::bind(&composite::summand2_changing, this,
                                       std::placeholders::_1));
}

void composite::summand1_changing(const int& value)
{
    sum_.value(value + summand2_.value());
}

void composite::summand2_changing(const int& value)
{
    sum_.value(summand1_.value() + value);
}
