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

#include "xml_visitor.h"

#include "basic_parameter.h"
#include "basic_readwrite_parameter.h"
#include "event.h"
#include "node.h"

namespace
{

static std::string userlevel_names[] = {
    "readonly",
    "normal",
    "service",
    "maintenance",
    "internal"
};

std::string node_type_str(decof::node *node)
{
    std::stringstream ss;
    ss << node->name() << std::hex << std::uppercase << node;
    return ss.str();
}

} // Anonymous namespace

namespace decof
{

xml_visitor::xml_visitor(std::stringstream &ss) :
    ss_(ss)
{
    ss_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<system name=\"DeCoF2 server\" version=\"1.0\">\n";

    increment_indentation();
    ss_ << indentation() << "<description> </description>\n";
}

xml_visitor::~xml_visitor()
{
    first_pass_ = false;

    for (; node_stack_.size() > 0; node_stack_.pop()) {
        auto node = node_stack_.top();

        if (node->parent() == nullptr)
            ss_ << indentation() << "<module name=\"" << node->name() << "\">\n";
        else
            ss_ << indentation() << "<xtypedef name=\"" << node_type_str(node) << "\">\n";

        increment_indentation();
        ss_ << indentation() << "<description> </description>\n";

        // Iterate child parameters
        for (auto &child : *node)
            child->accept(this);

        decrement_indentation();

        if (node->parent() == nullptr)
            ss_ << indentation() << "</module>\n";
        else
            ss_ << indentation() << "</xtypedef>\n";
    }

    ss_ << "</system>\n";
}

void xml_visitor::visit(event *ev)
{
    if (!first_pass_) {
        ss_ << indentation() << "<cmd name=\"" << ev->name() << "\""
            << " execlevel=\"" << userlevel_names[ev->writelevel()] << "\">"
            << "<description> </description></cmd>\n";
    }
}

void xml_visitor::visit(node *node)
{
    if (first_pass_)
        node_stack_.push(node);
    else
        write_param(node, node_type_str(node));
}

void xml_visitor::visit(basic_parameter*)
{}

void xml_visitor::visit(typed_parameter<boolean> *param)
{
    write_param(param, "BOOLEAN");
}

void xml_visitor::visit(typed_parameter<integer> *param)
{
    write_param(param, "INTEGER");
}

void xml_visitor::visit(typed_parameter<real> *param)
{
    write_param(param, "REAL");
}

void xml_visitor::visit(typed_parameter<string> *param)
{
    write_param(param, "STRING");
}

void xml_visitor::visit(typed_parameter<binary> *param)
{
    write_param(param, "BINARY");
}

void xml_visitor::visit(typed_parameter<boolean_seq> *param)
{
    write_param(param, "BOOLEAN_SEQ");
}

void xml_visitor::visit(typed_parameter<integer_seq> *param)
{
    write_param(param, "INTEGER_SEQ");
}

void xml_visitor::visit(typed_parameter<real_seq> *param)
{
    write_param(param, "REAL_SEQ");
}

void xml_visitor::visit(typed_parameter<string_seq> *param)
{
    write_param(param, "STRING_SEQ");
}

void xml_visitor::visit(typed_parameter<binary_seq> *param)
{
    write_param(param, "BINARY_SEQ");
}

void xml_visitor::write_param(basic_parameter *param, const std::string &type_str)
{
    if (!first_pass_) {
        bool readonly = (dynamic_cast<basic_readwrite_parameter*>(param) == nullptr);
        bool node = (dynamic_cast<decof::node*>(param) != nullptr);

        ss_ << indentation() << "<param name=\"" << param->name() << "\" "
            << "type=\"" << type_str << "\""
            << (node ? "" : std::string(" mode=\"") + (readonly ? "readonly" : "readwrite") + "\"")
            << (readonly ? "" : std::string(" readlevel=\"") + userlevel_names[param->readlevel()] + "\" " + "writelevel=\"" + userlevel_names[param->writelevel()] + "\"")
            << "><description> </description></param>\n";
    }
}

} // namespace decof
