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

#include <client_context/object_visitor.h>

#include "event.h"
#include "node.h"
#include "basic_parameter.h"

namespace decof
{

void object_visitor::visit(event* event)
{
//    visit(static_cast<object*>(event));
}

void object_visitor::visit(node* node)
{
//    visit(static_cast<object*>(node));
}

void object_visitor::visit(client_write_interface* param)
{
//    visit(static_cast<object*>(param));
}

void object_visitor::visit(client_read_interface* param)
{
//    visit(static_cast<object*>(param));
}

//void object_visitor::visit(basic_parameter<boolean> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<integer> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<real> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<string> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<binary> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<boolean_seq> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<integer_seq> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<real_seq> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<string_seq> *param)
//{
//    visit(static_cast<object*>(param));
//}

//void object_visitor::visit(basic_parameter<binary_seq> *param)
//{
//    visit(static_cast<object*>(param));
//}

void object_visitor::write_indentation(std::stringstream& ss, const object *te)
{
    ss << indentation();
    for (object *it = te->parent(); it != nullptr; it = it->parent())
        ss << "  ";
}

std::string object_visitor::indentation()
{
    std::string retval;
    for (size_t i = 0; i < indentation_baselevel_; ++i)
        retval += "  ";
    return retval;
}

void object_visitor::increment_indentation(size_t levels)
{
    indentation_baselevel_ += levels;
}

void object_visitor::decrement_indentation(size_t levels)
{
    if (indentation_baselevel_ >= levels)
        indentation_baselevel_ -= levels;
    else
        indentation_baselevel_ = 0u;
}

} // namespace decof
