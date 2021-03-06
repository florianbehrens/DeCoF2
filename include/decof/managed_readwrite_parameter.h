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

#ifndef DECOF_MANAGED_READWRITE_PARAMETER_H
#define DECOF_MANAGED_READWRITE_PARAMETER_H

#include "encoding_hint.h"
#include "observable_parameter.h"
#include "typed_client_write_interface.h"

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_MANAGED_READWRITE_PARAMETER(type_name, value_type)                                      \
    struct type_name : public decof::managed_readwrite_parameter<value_type>                                  \
    {                                                                                                         \
        type_name(                                                                                            \
            const char*        name,                                                                          \
            decof::node*       parent,                                                                        \
            decof::userlevel_t readlevel  = decof::Normal,                                                    \
            decof::userlevel_t writelevel = decof::Normal,                                                    \
            const value_type&  value      = value_type())                                                     \
          : decof::managed_readwrite_parameter<value_type>(name, parent, readlevel, writelevel, value)        \
        {                                                                                                     \
        }                                                                                                     \
        type_name(const char* name, decof::node* parent, const value_type& value)                             \
          : decof::managed_readwrite_parameter<value_type>(name, parent, decof::Normal, decof::Normal, value) \
        {                                                                                                     \
        }                                                                                                     \
        virtual void verify(const value_type& value) override;                                                \
    }

namespace decof {

/**
 * @brief A managed_readwrite_parameter may only be modified by the client side.
 *
 * This parameter type can be monitored efficiently.
 *
 * @tparam T The parameter value type.
 * @tparam EncodingHint A hint for value encoding.
 */
template <typename T, encoding_hint EncodingHint = encoding_hint::none>
class managed_readwrite_parameter : public observable_parameter<T, EncodingHint>,
                                    public typed_client_write_interface<T, EncodingHint>
{
  public:
    managed_readwrite_parameter(const char* name, node* parent, const T& value)
      : observable_parameter<T, EncodingHint>(name, parent, Normal, Normal), value_(value)
    {
    }

    managed_readwrite_parameter(
        const char* name,
        node*       parent,
        userlevel_t readlevel  = Normal,
        userlevel_t writelevel = Normal,
        const T&    value      = T())
      : observable_parameter<T, EncodingHint>(name, parent, readlevel, writelevel), value_(value)
    {
    }

    virtual T value() const override final
    {
        return value_;
    }

    /** @brief Access parameter value by constant reference.
     *
     * @return Constant reference to parameter value.
     *
     * @note Make sure the returned reference does not outlive the parameter
     * object itself. */
    const T& value_ref()
    {
        return value_;
    }

  protected:
    virtual void verify(const T&)
    {
    }

  private:
    virtual void value(const T& value) override final
    {
        if (value_ == value)
            return;

        verify(value);
        value_ = value;
        observable_parameter<T, EncodingHint>::emit(value);
    }

    T value_;
};

} // namespace decof

#endif // DECOF_MANAGED_READWRITE_PARAMETER_H
