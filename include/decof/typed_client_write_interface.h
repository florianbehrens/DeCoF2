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

#ifndef DECOF_TYPED_CLIENT_WRITE_INTERFACE_H
#define DECOF_TYPED_CLIENT_WRITE_INTERFACE_H

#include "client_write_interface.h"
#include "conversion.h"
#include "exceptions.h"

namespace decof
{

template<typename T>
class typed_client_write_interface : public client_write_interface
{
private:
    virtual void value(const T &value) = 0;
    virtual void value(const boost::any& any_value) override final
    {
        try {
            value(Conversion<T>::from_any(any_value));
        } catch(boost::bad_any_cast&) {
            throw wrong_type_error();
        }
    }
};

// Partial template specialization for sequence types.
template<typename T>
class typed_client_write_interface<decof::sequence<T>> : public client_write_interface
{
    friend class client_context;

private:
    virtual void value(const decof::sequence<T> &value) = 0;
    virtual void value(const boost::any& any_value) override final
    {
        try {
            const std::vector<boost::any> &any_vector = boost::any_cast<const std::vector<boost::any> &>(any_value);
            decof::sequence<T> new_value;
            new_value.reserve(any_vector.size());
            for (auto elem : any_vector)
                new_value.push_back(boost::any_cast<T>(elem));
            value(new_value);
        } catch(boost::bad_any_cast&) {
            throw wrong_type_error();
        }
    }
};

} // namespace decof

#endif // DECOF_TYPED_CLIENT_WRITE_INTERFACE_H
