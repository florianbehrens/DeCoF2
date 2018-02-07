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

#include "browse_visitor.h"

#include <boost/variant/apply_visitor.hpp>

#include <decof/client_read_interface.h>
#include <decof/node.h>

#include "encoder.h"

namespace decof
{

namespace cli
{

browse_visitor::browse_visitor(std::ostream &out) :
    out_(out)
{}

void browse_visitor::visit(object* obj)
{
    auto param = dynamic_cast<client_read_interface*>(obj);
    if (!param) return;

    write_indentation(out_, obj);
    out_ << (obj->parent() != nullptr ? ":" : "" )
        << obj->name();

    out_ << " = ";
    boost::apply_visitor(encoder(out_), static_cast<const value_t>(param->generic_value()));

    out_ << std::endl;
}

void browse_visitor::visit(node* node)
{
    write_indentation(out_, node);
    if (node->parent() != nullptr)
        out_ << ":";
    out_ << node->name() << std::endl;
}

} // namespace cli

} // namespace decof
