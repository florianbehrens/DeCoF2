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

#include <iomanip>

#include "endian.h"
#include "js_value_encoder.h"

namespace decof
{

namespace scgi
{

namespace {

/**
 * @brief Visitor class for sequence element encoding.
 */
class sequence_element_encoder : public boost::static_visitor<>
{
public:
    explicit sequence_element_encoder(std::ostream& out) :
        m_out(out)
    {}

    void operator()(const boolean& arg) const
    {
        m_out.put(static_cast<unsigned char>(arg));
    }

    void operator()(const integer& arg) const
    {
        int32_t elem_le = native_to_little_endian(arg);
        m_out.write(reinterpret_cast<const char*>(&elem_le), sizeof(elem_le));
    }

    void operator()(const real& arg) const
    {
        double elem_le = native_to_little_endian(arg);
        m_out.write(reinterpret_cast<const char*>(&elem_le), sizeof(elem_le));
    }

    void operator()(const string& arg) const
    {
        // Bencode (see http://en.wikipedia.org/wiki/Bencode) encoder
        m_out << arg.size() << ":" << arg << "\r\n";
    }

private:
    std::ostream& m_out;
};

} // Anonymous namespace

js_value_encoder::js_value_encoder(std::ostream& out) :
    m_out(out)
{}

void js_value_encoder::operator()(const generic_scalar &arg) const
{
    boost::apply_visitor(*this, arg);
}

void js_value_encoder::operator()(const sequence_t& arg) const
{
    sequence_element_encoder enc(m_out);
    for (const auto& elem : arg.value) boost::apply_visitor(enc, elem);
}

void js_value_encoder::operator()(const tuple_t& arg) const
{
    for (const auto& elem : arg.value) {
        boost::apply_visitor(*this, elem);
        m_out << "\r\n";
    }
}

void js_value_encoder::operator()(const boolean& arg) const
{
    m_out << std::boolalpha << arg << std::noboolalpha;
}

void js_value_encoder::operator()(const integer& arg) const
{
    m_out << arg;
}

void js_value_encoder::operator()(const real& arg) const
{
    // Use minimum value without loss of precision in conversion between binary
    // and decimal number representation and vice versa.
    m_out << std::setprecision(17) << arg;
}

void js_value_encoder::operator()(const string& arg) const
{
    m_out << arg;
}

} // namespace scgi

} // namespace decof
