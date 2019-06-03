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

#ifndef DECOF_SCGI_JS_VALUE_ENCODER_H
#define DECOF_SCGI_JS_VALUE_ENCODER_H

#include <decof/types.h>
#include <boost/variant/static_visitor.hpp>
#include <ostream>

namespace decof {

namespace scgi {

struct js_value_encoder
{
    explicit js_value_encoder(std::ostream& out);

    void operator()(const scalar_t& arg) const;
    void operator()(const sequence_t& arg) const;
    void operator()(const tuple_t& arg) const;

    void operator()(const boolean_t& arg) const;
    void operator()(const integer_t& arg) const;
    void operator()(const real_t& arg) const;
    void operator()(const string_t& arg) const;
    void operator()(const binary_t& arg) const;

  private:
    std::ostream& m_out;
};

} // namespace scgi

} // namespace decof

#endif // DECOF_SCGI_JS_VALUE_ENCODER_H
