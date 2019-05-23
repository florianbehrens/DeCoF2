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

#include "object.h"
#include "node.h"
#include "object_dictionary.h"
#include <cassert>

namespace decof {

object::object(std::string name, node* parent, userlevel_t readlevel, userlevel_t writelevel)
  : name_(name), parent_(parent), readlevel_(readlevel), writelevel_(writelevel)
{
    assert(readlevel != Readonly);
    assert(writelevel != Readonly);

    if (parent != nullptr)
        parent->add_child(this);
}

object::~object()
{
    if (parent_)
        parent_->remove_child(this);
}

std::string object::name() const
{
    return name_;
}

void object::name(std::string name)
{
    name_ = name;
}

std::string object::fq_name() const
{
    if (!parent_)
        return name_;

    return parent_->fq_name() + std::string(":") + name_;
}

node* object::parent() const
{
    return parent_;
}

void object::reset_parent(node* parent)
{
    if (parent_)
        parent_->remove_child(this);

    if (parent)
        parent->add_child(this);
}

userlevel_t object::readlevel() const
{
    return readlevel_;
}

void object::readlevel(userlevel_t readlevel)
{
    readlevel_ = readlevel;
}

userlevel_t object::writelevel() const
{
    return writelevel_;
}

void object::writelevel(userlevel_t writelevel)
{
    writelevel_ = writelevel;
}

void object::accept(object_visitor* visitor)
{
    visitor->visit(this);
}

object_dictionary* object::get_object_dictionary()
{
    object* te = this;
    while (te->parent() != nullptr)
        te = te->parent();

    return dynamic_cast<object_dictionary*>(te);
}

const object_dictionary* object::get_object_dictionary() const
{
    const object* te = this;
    while (te->parent() != nullptr)
        te = te->parent();

    return dynamic_cast<const object_dictionary*>(te);
}

} // namespace decof
