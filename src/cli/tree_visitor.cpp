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

void tree_visitor::visit(decof::event *event)
{
    out_ << event->fq_name() << " EVENT\n";
}

void tree_visitor::visit(decof::node *node)
{
    out_ << node->fq_name() << " NODE\n";
}

void tree_visitor::visit(decof::object *obj)
{
    out_ << obj->fq_name() << " UNKNOWN\n";
}

void tree_visitor::visit(decof::basic_parameter<boolean> *param)
{
    write_param(param, "BOOLEAN");
}

void tree_visitor::visit(decof::basic_parameter<integer> *param)
{
    write_param(param, "INTEGER");
}

void tree_visitor::visit(decof::basic_parameter<real> *param)
{
    write_param(param, "REAL");
}

void tree_visitor::visit(decof::basic_parameter<string> *param)
{
    write_param(param, "STRING");
}

void tree_visitor::visit(decof::basic_parameter<binary> *param)
{
    write_param(param, "BINARY");
}

void tree_visitor::visit(decof::basic_parameter<boolean_seq> *param)
{
    write_param(param, "BOOLEAN_SEQ");
}

void tree_visitor::visit(decof::basic_parameter<integer_seq> *param)
{
    write_param(param, "INTEGER_SEQ");
}

void tree_visitor::visit(decof::basic_parameter<real_seq> *param)
{
    write_param(param, "REAL_SEQ");
}

void tree_visitor::visit(decof::basic_parameter<string_seq> *param)
{
    write_param(param, "STRING_SEQ");
}

void tree_visitor::visit(decof::basic_parameter<binary_seq> *param)
{
    write_param(param, "BINARY_SEQ");
}

void tree_visitor::write_param(decof::object *obj, const char *type)
{
    out_ << obj->fq_name() << " PARAM " << (obj->writelevel() == Forbidden ? "RO" : "RW") << " " << type << "\n";
}

} // namespace cli

} // namespace decof
