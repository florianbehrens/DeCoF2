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

#ifndef DECOF_WRITEONLY_HANDLER_PARAMETER_H
#define DECOF_WRITEONLY_HANDLER_PARAMETER_H

#include "writeonly_parameter.h"
#include <functional>

namespace decof {

/** @brief Writeonly parameter class with handler registration support.
 *
 * This class is the handler version of class #writeonly_parameter. It
 * supports runtime registration of callable(s) as handler functions.
 *
 * @tparam T The value type.
 */
template <typename T>
class writeonly_handler_parameter : public writeonly_parameter<T>
{
  public:
    using writeonly_parameter<T>::writeonly_parameter;

    /// Set external value setter handler.
    void value_handler(std::function<void(const T&)> handler)
    {
        value_handler_ = handler;
    }

  private:
    virtual void value(const T& value) override final
    {
        if (value_handler_)
            return value_handler_(value);
    }

    std::function<void(const T&)> value_handler_;
};

} // namespace decof

#endif // DECOF_WRITEONLY_HANDLER_PARAMETER_H
