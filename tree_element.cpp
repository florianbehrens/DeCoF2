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

#include "tree_element.h"

#include "node.h"
#include "object_dictionary.h"

namespace decof
{

tree_element::tree_element(std::string name, node *parent)
 : name_(name), parent_(parent)
{
    if (parent != nullptr)
        parent->add_child(this);
}

tree_element::~tree_element()
{
    if (parent_ != nullptr)
        parent_->remove_child(this);
}

std::string tree_element::name() const
{
    return name_;
}

void tree_element::set_name(std::string name)
{
    name_ = name;
}

std::string tree_element::fq_name() const
{
    if (parent_ == nullptr)
        return name_;

    return parent_->fq_name() + std::string(":") + name_;
}

node *tree_element::parent() const
{
    return parent_;
}

void tree_element::set_parent(node *parent)
{
    if (parent_ != nullptr)
        parent->remove_child(this);
    parent_ = parent;
}

object_dictionary *tree_element::get_object_dictionary()
{
    tree_element* te = this;
    while (te->parent() != nullptr)
        te = te->parent();

    return dynamic_cast<object_dictionary*>(te);
}

} // namespace decof
