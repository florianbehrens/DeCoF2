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

#ifndef DECOF_NODE_H
#define DECOF_NODE_H

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "types.h"
#include "observable_parameter.h"

namespace decof
{

class node : public observable_parameter<string_seq>
{
public:
    typedef std::list<tree_element*> children_t;
    typedef children_t::iterator iterator;

public:
    explicit node(std::string name, node *parent = nullptr, userlevel_t readlevel = Readonly);
    virtual ~node();

    virtual string_seq value() override;

    void add_child(tree_element *child);
    void remove_child(tree_element *child);
    tree_element *find_child(const std::string &uri);

    /// The same as value().
    string_seq children();

private:
    tree_element *find_immediate_child(const std::string &name);

    // We use std::list because iterators of a list remain valid when elements
    // are deleted.
    children_t children_;
};

} // namespace decof

#endif // DECOF_NODE_H