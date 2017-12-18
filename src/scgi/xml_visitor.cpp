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

#include <map>

#include <decof/client_write_interface.h>
#include <decof/event.h>
#include <decof/node.h>

namespace
{

std::string node_type_str(decof::node *node)
{
    std::stringstream ss;
    ss << node->name() << std::hex << std::uppercase << node;
    return ss.str();
}

std::string userlevel_name(decof::userlevel_t ul)
{
    static const std::map<decof::userlevel_t, const char*> userlevel_names = {
        { decof::Internal,    "internal" },
        { decof::Service,     "service" },
        { decof::Maintenance, "maintenance" },
        { decof::Normal,      "normal" },
        { decof::Readonly,    "readonly" }
    };

    try {
        return userlevel_names.at(ul);
    } catch (std::out_of_range&) {}

    return "invalid";
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

        // Generate artificial 'ul' parameter and 'change-ul' command
        if (node->parent() == nullptr) {
            out_ << indentation() << "<param name=\"ul\" type=\"INTEGER\" mode=\"readonly\"><description> </description></param>\n";
            out_ << indentation() <<
                    "<cmd name=\"change-ul\">"
                        "<description> </description>"
                        "<ret type=\"INTEGER\" />"
                        "<arg name=\"ul\" type=\"INTEGER\" />"
                        "<arg name=\"passwd\" type=\"STRING\" />"
                    "</cmd>\n";
        }

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

void xml_visitor::visit(event* event)
{
    if (!first_pass_) {
        out_ << indentation() << "<cmd name=\"" << event->name() << "\""
            << " execlevel=\"" << userlevel_name(event->writelevel()) << "\">"
            << "<description> </description></cmd>\n";
    }
}

void xml_visitor::visit(node* node)
{
    if (first_pass_)
        node_stack_.push(node);
    else
        write_param(node, node_type_str(node));
}

void xml_visitor::visit(object* obj, boolean_tag)
{
    write_param(obj, "BOOLEAN");
}

void xml_visitor::visit(object* obj, integer_tag)
{
    write_param(obj, "INTEGER");
}

void xml_visitor::visit(object* obj, real_tag)
{
    write_param(obj, "REAL");
}

void xml_visitor::visit(object* obj, string_tag)
{
    write_param(obj, "STRING");
}

void xml_visitor::visit(object* obj, binary_tag)
{
    write_param(obj, "BINARY");
}

void xml_visitor::visit(object* obj, sequence_tag<boolean_tag>)
{
    write_param(obj, "BOOLEAN_SEQ");
}

void xml_visitor::visit(object* obj, sequence_tag<integer_tag>)
{
    write_param(obj, "INTEGER_SEQ");
}

void xml_visitor::visit(object* obj, sequence_tag<real_tag>)
{
    write_param(obj, "REAL_SEQ");
}

void xml_visitor::visit(object* obj, sequence_tag<string_tag>)
{
    write_param(obj, "STRING_SEQ");
}

void xml_visitor::visit(object* obj, tuple_tag)
{
    write_param(obj, "TUPLE");
}

void xml_visitor::write_param(const decof::object* obj, const std::string &type_str)
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

} // namespace scgi

} // namespace decof
