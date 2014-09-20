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

#ifndef READWRITE_PARAMETER_H
#define READWRITE_PARAMETER_H

#include "basic_readwrite_parameter.h"
#include "conversion.h"
#include "exceptions.h"

namespace decof
{

template<typename T>
class readwrite_parameter : public basic_readwrite_parameter
{
    friend class client_context;

private:
    virtual void set_private_value(const T &value) = 0;

    /// @brief Set value wrapped in a runtime dynamic type.
    /// Scalar and sequence types must be wrapped in a boost::any as they are.
    /// Tuple types must be dismantled and the individual elements wrapped in a
    /// vector of boost::anys which is, in turn, again wrapped in a boost::any.
    virtual void set_private_value(const boost::any& any_value)
    {
        try {
            set_private_value(Conversion<T>::from_any(any_value));
        } catch(boost::bad_any_cast&) {
            throw wrong_type_error();
        }
    }
};

// Partial template specialization
template<typename T>
class readwrite_parameter<std::vector<T>> : public basic_readwrite_parameter
{
    friend class client_context;

private:
    virtual void set_private_value(const std::vector<T> &value) = 0;
    virtual void set_private_value(const boost::any& any_value)
    {
        try {
            std::vector<boost::any> any_vector = boost::any_cast<std::vector<boost::any>>(any_value);
            std::vector<T> new_value;
            new_value.reserve(any_vector.size());
            for (auto elem : any_vector)
                new_value.push_back(boost::any_cast<T>(elem));
            set_private_value(new_value);
        } catch(boost::bad_any_cast&) {
            throw wrong_type_error();
        }
    }
};

} // namespace decof

#endif // READWRITE_PARAMETER_H
