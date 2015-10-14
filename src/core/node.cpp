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

#include <cassert>
#include <algorithm>
#include <iostream>

#include "object_visitor.h"

namespace decof
{

node::node(std::string name, node *parent, userlevel_t readlevel)
 : readable_parameter<string_seq>(name, parent, readlevel, Forbidden)
{}

node::~node()
{}

string_seq node::value()
{
    return children();
}

void node::accept(object_visitor *visitor)
{
    visitor->visit(this);
}

void node::add_child(object *child)
{
    // Check whether child already is registered
    assert(std::find_if(children_.cbegin(), children_.cend(), [child](const object *o) {
        return o == child;
    }) == children_.cend());

    children_.push_back(child);
}

void node::remove_child(object *child)
{
    children_.remove(child);
}

object *node::find_immediate_child(const std::string &name)
{
    children_t::iterator it = std::find_if(children_.begin(), children_.end(), [name](const children_t::value_type &value) {
        return value->name() == name;
    });

    if (it != children_.end())
        return *it;

    return nullptr;
}

object *node::find_child(const std::string &uri, char separator)
{
    if (uri.empty())
        return this;

    std::string::size_type idx = uri.find(separator);
    std::string child_name = uri.substr(0, idx);
    std::string sub_uri;

    if (idx != std::string::npos)
        sub_uri = uri.substr(idx + 1, uri.length());

    // Find immediate child element
    object *te = find_immediate_child(child_name);

    // Check whether child element is a node
    node *child_node = dynamic_cast<node *>(te);
    if (child_node != nullptr) {
        return child_node->find_child(sub_uri, separator);
    }

    return te;
}

string_seq node::children()
{
    string_seq retval;
    retval.reserve(children_.size());
    for (const auto c : children_)
        retval.push_back(c->name());
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
