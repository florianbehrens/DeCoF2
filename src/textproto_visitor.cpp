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

#include "basic_parameter.h"
#include "node.h"
#include "string_codec.h"

namespace decof
{

textproto_visitor::textproto_visitor(std::stringstream &ss) :
    ss_(ss)
{}

void textproto_visitor::visit(node *node)
{
    write_indentation(node);
    ss_ << node->name() << std::endl;
}

void textproto_visitor::visit(basic_parameter *param)
{
    write_indentation(param);
    ss_ << param->name() << " = " << string_codec::encode(param->any_value()) << std::endl;
}

void textproto_visitor::write_indentation(const tree_element *te)
{
    for (tree_element *it = te->parent(); it != nullptr; it = it->parent())
        ss_ << "  ";
    if (te->parent() != nullptr)
        ss_ << ":";
}

} // namespace decof
