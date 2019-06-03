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

#ifndef DECOF_EXTERNAL_READONLY_PARAMETER_H
#define DECOF_EXTERNAL_READONLY_PARAMETER_H

#include "encoding_hint.h"
#include "object_dictionary.h"
#include "observable_parameter.h"
#include "tick_interface.h"
#include <boost/signals2/connection.hpp>
#include <optional>
#include <string>

/// Convenience macro for parameter declaration
#define DECOF_DECLARE_EXTERNAL_READONLY_PARAMETER(type_name, value_type)                               \
    struct type_name : public decof::external_readonly_parameter<value_type>                           \
    {                                                                                                  \
        type_name(std::string name, decof::node* parent, decof::userlevel_t readlevel = decof::Normal) \
          : decof::external_readonly_parameter<value_type>(name, parent, readlevel)                    \
        {                                                                                              \
        }                                                                                              \
        virtual value_type external_value() const override;                                            \
    }

namespace decof {

/**
 * @brief Readonly parameter type with an externally managed value.
 *
 * An external_readonly_parameter may only be modified by the server side and is
 * managed by the server implementation, i.e. externally from the framework's
 * viewpoint.
 *
 * An external_readonly_parameter is the only parameter type that internally
 * uses a polling timer for monitoring.
 *
 * @tparam T The parameter value type.
 * @tparam EncodingHint A hint for value encoding.
 */
template <typename T, encoding_hint EncodingHint = encoding_hint::none>
class external_readonly_parameter : public tick_interface, public observable_parameter<T, EncodingHint>
{
  public:
    external_readonly_parameter(std::string name, node* parent, userlevel_t readlevel = Normal)
      : observable_parameter<T, EncodingHint>(name, parent, readlevel, Forbidden)
    {
    }

    virtual T value() const override final
    {
        return external_value();
    }

    /// @brief Call this member function to signal value changes.
    /// In cases where a value change information is obtained by external means,
    /// (e.g., from a select() on a file descriptor) calling this member
    /// function results in a value update for subscribers.
    void value_changed()
    {
        notify();
    }

    virtual boost::signals2::scoped_connection observe(value_change_slot slot) override final
    {
        auto od = this->get_object_dictionary();

        if (od == nullptr) {
            return boost::signals2::scoped_connection();
        }

        if (observations_++ == 0) {
            od->register_for_tick(this);
        }

        // Call base class member function
        return observable_parameter<T, EncodingHint>::observe(slot);
    }

    virtual void unobserve() override
    {
        if (--observations_ == 0) {
            if (auto od = this->get_object_dictionary()) {
                od->unregister_for_tick(this);
            }
        }
    }

  private:
    virtual T external_value() const = 0;

    virtual void tick() override
    {
        notify();
    }

    /// Slot member function for regular tick.
    void notify()
    {
        T cur_value = value();
        if (!last_value_ || *last_value_ != cur_value) {
            observable_parameter<T, EncodingHint>::emit(cur_value);
            last_value_ = cur_value;
        }
    }

    std::size_t      observations_{0};
    std::optional<T> last_value_;
};

} // namespace decof

#endif // DECOF_EXTERNAL_READONLY_PARAMETER_H
