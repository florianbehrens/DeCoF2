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

#ifndef NODE_H
#define NODE_H

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "container_types.h"
#include "observable_parameter.h"

namespace decof
{

class node : public observable_parameter<string_vector>
{
public:
    typedef std::list<tree_element*> children_t;
    typedef children_t::iterator iterator;

public:
    explicit node(std::string name, node *parent = nullptr);
    virtual ~node();

    virtual string_vector value() override;

    void add_child(tree_element *child);
    void remove_child(tree_element *child);
    tree_element *find_child(std::string uri);

    /// The same as value().
    string_vector children();

private:
    tree_element *find_immediate_child(const std::string &name);

    // We use std::list because iterators of a list remain valid when elements
    // are deleted.
    children_t children_;
};

} // namespace decof

#endif // NODE_H
