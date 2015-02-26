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

#include "textproto_visitor.h"

#include "node.h"
#include "textproto_encoder.h"

namespace decof
{

textproto_visitor::textproto_visitor(std::stringstream &ss) :
    ss_(ss)
{}

void textproto_visitor::visit(node *node)
{
    write_indentation(ss_, node);
    if (node->parent() != nullptr)
        ss_ << ":";
    ss_ << node->name() << std::endl;
}

void textproto_visitor::visit(object *obj)
{
    client_read_interface *read_if = dynamic_cast<client_read_interface*>(obj);

    write_indentation(ss_, obj);

    ss_ << (obj->parent() != nullptr ? ":" : "" )
        << obj->name();
    if (read_if != nullptr) {
        ss_ << " = ";
        textproto_encoder().encode_any(ss_, read_if->any_value());
    }
    ss_ << std::endl;
}

} // namespace decof
