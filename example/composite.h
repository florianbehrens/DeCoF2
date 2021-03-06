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

#ifndef DECOF_EXAMPLE_COMPOSITE_H
#define DECOF_EXAMPLE_COMPOSITE_H

#include <decof/all.h>

class composite final : public decof::node
{
    friend class sum_type;

  public:
    explicit composite(const char* name, node* parent = nullptr);

  private:
    void summand1_changing(const int& value);
    void summand2_changing(const int& value);

    decof::managed_readwrite_handler_parameter<int> summand1_;
    decof::managed_readwrite_handler_parameter<int> summand2_;
    decof::managed_readonly_parameter<int>          sum_;
};

#endif // DECOF_EXAMPLE_COMPOSITE_H
