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

#include "tree_visitor.h"

#include <decof/event.h>
#include <decof/node.h>
#include <decof/object.h>

#include "encoder.h"

namespace decof
{

namespace cli
{

tree_visitor::tree_visitor(std::ostream& out) :
    out_(out)
{}

void tree_visitor::visit(object *obj)
{
    out_ << obj->fq_name() << " UNKNOWN\n";
}

void tree_visitor::visit(event *event)
{
    out_ << event->fq_name() << " EVENT\n";
}

void tree_visitor::visit(node *node)
{
    out_ << node->fq_name() << " NODE\n";
}

void tree_visitor::visit(object* obj, boolean_tag)
{
    write_param(obj, "BOOLEAN");
}

void tree_visitor::visit(object* obj, integer_tag)
{
    write_param(obj, "INTEGER");
}

void tree_visitor::visit(object* obj, real_tag)
{
    write_param(obj, "REAL");
}

void tree_visitor::visit(object* obj, string_tag)
{
    write_param(obj, "STRING");
}

void tree_visitor::visit(object* obj, binary_tag)
{
    write_param(obj, "BINARY");
}

void tree_visitor::visit(object* obj, sequence_tag<boolean_tag>)
{
    write_param(obj, "BOOLEAN_SEQ");
}

void tree_visitor::visit(object* obj, sequence_tag<integer_tag>)
{
    write_param(obj, "INTEGER_SEQ");
}

void tree_visitor::visit(object* obj, sequence_tag<real_tag>)
{
    write_param(obj, "REAL_SEQ");
}

void tree_visitor::visit(object* obj, sequence_tag<string_tag>)
{
    write_param(obj, "STRING_SEQ");
}

void tree_visitor::visit(object* obj, tuple_tag)
{
    write_param(obj, "TUPLE");
}

void tree_visitor::write_param(decof::object *obj, const char *type)
{
    out_ << obj->fq_name() << " PARAM " << (obj->writelevel() == Forbidden ? "RO" : "RW") << " " << type << "\n";
}

} // namespace cli

} // namespace decof
