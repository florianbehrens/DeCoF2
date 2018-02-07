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

#ifndef DECOF_MANAGED_READWRITE_HANDLER_PARAMETER_H
#define DECOF_MANAGED_READWRITE_HANDLER_PARAMETER_H

#include <functional>

#include "encoding_hint.h"
#include "managed_readwrite_parameter.h"

namespace decof
{

/**
 * @brief Handler-based managed readwrite parameter class.
 *
 * This class is the handler version of class #managed_readwrite_parameter. It
 * supports runtime registration of callable(s) as handler function(s).
 *
 * @tparam T The parameter value type.
 * @tparam EncodingHint A hint for value encoding.
 */
template<typename T, encoding_hint EncodingHint = encoding_hint::none>
class managed_readwrite_handler_parameter : public managed_readwrite_parameter<T, EncodingHint>
{
public:
    using managed_readwrite_parameter<T, EncodingHint>::managed_readwrite_parameter;

    managed_readwrite_handler_parameter(const std::string &name, node *parent,
                                        std::function<void(const T&)> verify_handler,
                                        const T &value) :
        managed_readwrite_parameter<T, EncodingHint>(name, parent, Normal, Normal, value),
        verify_handler_(verify_handler)
    {}

    managed_readwrite_handler_parameter(const std::string &name, node *parent,
                                        std::function<void(const T&)> verify_handler,
                                        userlevel_t readlevel = Normal, userlevel_t writelevel = Normal,
                                        const T &value = T()) :
        managed_readwrite_parameter<T, EncodingHint>(name, parent, readlevel, writelevel, value),
        verify_handler_(verify_handler)
    {}

    /// Set verify handler.
    void verify_handler(std::function<void(const T&)> handler)
    {
        verify_handler_ = handler;
    }

private:
    virtual void verify(const T &value) override final
    {
        if (verify_handler_) verify_handler_(value);
    }

    std::function<void(const T&)> verify_handler_;
};

} // namespace decof

#endif // DECOF_MANAGED_READWRITE_HANDLER_PARAMETER_H
