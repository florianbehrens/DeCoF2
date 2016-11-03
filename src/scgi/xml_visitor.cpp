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

#include <decof/client_write_interface.h>
#include <decof/event.h>
#include <decof/node.h>

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

namespace scgi
{

xml_visitor::xml_visitor(std::ostream &out) :
    out_(out)
{
    out_ << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        << "<system name=\"DeCoF2 server\" version=\"1.0\">\n";

    increment_indentation();
    out_ << indentation() << "<description> </description>\n";
}

xml_visitor::~xml_visitor()
{
    first_pass_ = false;

    for (; node_stack_.size() > 0; node_stack_.pop()) {
        auto node = node_stack_.top();

        if (node->parent() == nullptr)
            out_ << indentation() << "<module name=\"" << node->name() << "\">\n";
        else
            out_ << indentation() << "<xtypedef name=\"" << node_type_str(node) << "\">\n";

        increment_indentation();
        out_ << indentation() << "<description> </description>\n";

        // Iterate child parameters
        for (auto &child : *node)
            child->accept(this);

        decrement_indentation();

        if (node->parent() == nullptr)
            out_ << indentation() << "</module>\n";
        else
            out_ << indentation() << "</xtypedef>\n";
    }

    out_ << "</system>\n";
}

void xml_visitor::visit(event *ev)
{
    if (!first_pass_) {
        out_ << indentation() << "<cmd name=\"" << ev->name() << "\""
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

void xml_visitor::visit(object*)
{}

void xml_visitor::visit(basic_parameter<boolean> *param)
{
    write_param(param, "BOOLEAN");
}

void xml_visitor::visit(basic_parameter<integer> *param)
{
    write_param(param, "INTEGER");
}

void xml_visitor::visit(basic_parameter<real> *param)
{
    write_param(param, "REAL");
}

void xml_visitor::visit(basic_parameter<string> *param)
{
    write_param(param, "STRING");
}

void xml_visitor::visit(basic_parameter<binary> *param)
{
    write_param(param, "BINARY");
}

void xml_visitor::visit(basic_parameter<boolean_seq> *param)
{
    write_param(param, "BOOLEAN_SEQ");
}

void xml_visitor::visit(basic_parameter<integer_seq> *param)
{
    write_param(param, "INTEGER_SEQ");
}

void xml_visitor::visit(basic_parameter<real_seq> *param)
{
    write_param(param, "REAL_SEQ");
}

void xml_visitor::visit(basic_parameter<string_seq> *param)
{
    write_param(param, "STRING_SEQ");
}

void xml_visitor::visit(basic_parameter<binary_seq> *param)
{
    write_param(param, "BINARY_SEQ");
}

void xml_visitor::write_param(const object *obj, const std::string &type_str)
{
    if (!first_pass_) {
        bool readonly = (dynamic_cast<const client_write_interface*>(obj) == nullptr);

        out_ << indentation() << "<param name=\"" << obj->name() << "\" " << "type=\"" << type_str << "\"";

        if (dynamic_cast<const decof::node*>(obj) == nullptr) {
            out_ << std::string(" mode=\"");

            if (readonly)
                out_ << "readonly";
            else
                out_ << "readwrite";

            out_ << "\"";
        }

        if (readonly == false)
            out_ << " readlevel=\"" << userlevel_name(obj->readlevel()) << "\" writelevel=\"" << userlevel_name(obj->writelevel()) << "\"";

        out_ << "><description> </description></param>\n";
    }
}

std::string xml_visitor::userlevel_name(userlevel_t ul) const
{
    if (ul < sizeof(userlevel_names) / sizeof(std::string))
        return userlevel_names[ul];
    return "Invalid";
}

} // namespace scgi

} // namespace decof
