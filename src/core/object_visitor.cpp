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

#include "object_visitor.h"
#include "basic_parameter.h"
#include "event.h"
#include "node.h"

namespace decof {

void object_visitor::visit(event* e)
{
    visit(static_cast<object*>(e));
}

void object_visitor::visit(node* n)
{
    visit(static_cast<object*>(n));
}

void object_visitor::visit(object* obj, boolean_tag)
{
    visit(obj);
}

void object_visitor::visit(object* obj, integer_tag)
{
    visit(obj);
}

void object_visitor::visit(object* obj, real_tag)
{
    visit(obj);
}

void object_visitor::visit(object* obj, string_tag)
{
    visit(obj);
}

void object_visitor::visit(object* obj, binary_tag)
{
    visit(obj);
}

void object_visitor::visit(object* obj, sequence_tag<boolean_tag>)
{
    visit(obj);
}

void object_visitor::visit(object* obj, sequence_tag<integer_tag>)
{
    visit(obj);
}

void object_visitor::visit(object* obj, sequence_tag<real_tag>)
{
    visit(obj);
}

void object_visitor::visit(object* obj, sequence_tag<string_tag>)
{
    visit(obj);
}

void object_visitor::visit(object* obj, tuple_tag)
{
    visit(obj);
}

void object_visitor::write_indentation(std::ostream& out, const object* te)
{
    out << indentation();
    for (object* it = te->parent(); it != nullptr; it = it->parent())
        out << "  ";
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
