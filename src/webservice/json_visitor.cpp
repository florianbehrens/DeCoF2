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

#include "json_visitor.h"

#include "basic_parameter.h"
#include "basic_readwrite_parameter.h"
#include "event.h"
#include "node.h"

namespace decof
{

json_visitor::json_visitor(std::stringstream &ss) :
    ss_(ss),
    current_node_(nullptr)
{}

json_visitor::~json_visitor()
{
    ss_ << "}\n";
}

void json_visitor::visit(event *ev)
{
    check_indentation(ev);
    ss_ << indentation() << "\"" << ev->name() << "\": \"EVENT\"\n";
}

void json_visitor::visit(node *node)
{
    check_indentation(node);
    ss_ << indentation() << "\"" << node->name() << "\": {\n";
    current_node_ = node;
    increment_indentation();
}

void json_visitor::visit(basic_parameter *param)
{
    write_param(param, "UNKNOWN");
}

void json_visitor::visit(typed_parameter<boolean> *param)
{
    write_param(param, "BOOLEAN");
}

void json_visitor::visit(typed_parameter<integer> *param)
{
    write_param(param, "INTEGER");
}

void json_visitor::visit(typed_parameter<real> *param)
{
    write_param(param, "REAL");
}

void json_visitor::visit(typed_parameter<string> *param)
{
    write_param(param, "STRING");
}

void json_visitor::visit(typed_parameter<binary> *param)
{
    write_param(param, "BINARY");
}

void json_visitor::visit(typed_parameter<boolean_seq> *param)
{
    write_param(param, "BOOLEAN_SEQ");
}

void json_visitor::visit(typed_parameter<integer_seq> *param)
{
    write_param(param, "INTEGER_SEQ");
}

void json_visitor::visit(typed_parameter<real_seq> *param)
{
    write_param(param, "REAL_SEQ");
}

void json_visitor::visit(typed_parameter<string_seq> *param)
{
    write_param(param, "STRING_SEQ");
}

void json_visitor::visit(typed_parameter<binary_seq> *param)
{
    write_param(param, "BINARY_SEQ");
}

void json_visitor::check_indentation(object *obj)
{
    if (obj->parent() != current_node_) {
        current_node_ = obj->parent();
        decrement_indentation();
        ss_ << indentation() << "}\n";
    }
}

void json_visitor::write_param(basic_parameter *param, const std::string &type_str)
{
    ss_ << indentation() << "\"" << param->name() << "\": \"" << type_str << "\"\n";
}

} // namespace decof
