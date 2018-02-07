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

#ifndef DECOF_EXTERNAL_READWRITE_HANDLER_PARAMETER_H
#define DECOF_EXTERNAL_READWRITE_HANDLER_PARAMETER_H

#include <functional>

#include "encoding_hint.h"
#include "external_readwrite_parameter.h"

namespace decof
{

/**
 * @brief External readwrite parameter class with handler registration support.
 *
 * This class is the handler version of class #external_readwrite_parameter. It
 * supports runtime registration of callable(s) as handler functions.
 *
 * @tparam T The value type.
 * @tparam EncodingHint A hint for value encoding.
 */
template<typename T, encoding_hint EncodingHint = encoding_hint::none>
class external_readwrite_handler_parameter : public external_readwrite_parameter<T, EncodingHint>
{
public:
    using external_readwrite_parameter<T, EncodingHint>::external_readwrite_parameter;

    /// Set external value setter handler.
    external_readwrite_handler_parameter<T, EncodingHint> &external_value_set_handler(std::function<bool(const T&)> handler)
    {
        external_value_set_handler_ = handler;
        return *this;
    }

    /// Set external value getter handler.
    external_readwrite_handler_parameter<T, EncodingHint> &external_value_get_handler(std::function<T()> handler)
    {
        external_value_get_handler_ = handler;
        return *this;
    }

private:
    virtual bool external_value(const T &value) override final
    {
        if (external_value_set_handler_)
            return external_value_set_handler_(value);

        throw not_implemented_error();
    }

    virtual T external_value() const override final
    {
        if (external_value_get_handler_)
            return external_value_get_handler_();

        throw not_implemented_error();
    }

    std::function<bool(const T&)> external_value_set_handler_;
    std::function<T()> external_value_get_handler_;
};

} // namespace decof

#endif // DECOF_EXTERNAL_READWRITE_HANDLER_PARAMETER_H
