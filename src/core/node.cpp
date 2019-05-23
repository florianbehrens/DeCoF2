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

#include "node.h"
#include "object_visitor.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <list>
#include <string_view>

namespace decof {

node::node(std::string name, node* parent, userlevel_t readlevel)
  : readable_parameter<std::list<std::string>>(name, parent, readlevel, Forbidden)
{
}

node::~node()
{
    for (auto child : children_) {
        child->parent_ = nullptr;
    }
}

std::list<std::string> node::value() const
{
    return children();
}

void node::accept(object_visitor* visitor)
{
    visitor->visit(this);
}

void node::add_child(object* child)
{
    if (child->parent_)
        child->parent()->remove_child(child);

    children_.push_back(child);
    child->parent_ = this;
}

void node::remove_child(object* child)
{
    children_.remove_if([child](object* obj) {
        if (obj == child) {
            child->parent_ = nullptr;
            return true;
        }
        return false;
    });
}

object* node::find_child(std::string_view name)
{
    children_t::iterator it =
        std::find_if(children_.begin(), children_.end(), [name](const children_t::value_type& value) {
            return value->name() == name;
        });

    if (it != children_.end())
        return *it;

    return nullptr;
}

object* node::find_descendant_object(std::string_view uri, char separator)
{
    auto    sep_idx = uri.find(separator);
    object* obj     = find_child(uri.substr(0, sep_idx));

    uri.remove_prefix(std::min(sep_idx, uri.size()));

    if (uri.size() == 0) {
        return obj;
    }

    uri.remove_prefix(1);

    if (uri.size() == 0) {
        return nullptr;
    }

    if (auto child_node = dynamic_cast<node*>(obj)) {
        return child_node->find_descendant_object(uri, separator);
    }

    return nullptr;
}

std::list<std::string> node::children() const
{
    std::list<std::string> retval;
    std::transform(
        children_.cbegin(), children_.cend(), std::back_inserter(retval), [](object* obj) { return obj->name(); });
    return retval;
}

node::iterator node::begin()
{
    return children_.begin();
}

node::iterator node::end()
{
    return children_.end();
}

} // namespace decof
