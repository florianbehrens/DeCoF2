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

#include "automatic_ptr.h"
#include "readable_parameter.h"
#include "types.h"

namespace decof
{

class node : public automatic_ptr_target<node>, public readable_parameter<string_seq>
{
public:
    typedef std::list<object*> children_t;
    typedef children_t::iterator iterator;

public:
    explicit node(std::string name, node *parent = nullptr, userlevel_t readlevel = Normal);
    virtual ~node();

    virtual string_seq value() const override;

    /// Visitor pattern accept method
    virtual void accept(object_visitor *visitor) override final;

    /// @brief Add child #object to #node.
    /// @pre The child object must not have been added before. Adding the same child
    /// twice is considered an error and results in undefined behavior.
    /// @param child Child #object to be added to parent #node.
    /// @note The parent #node does not take ownership of the given child, i.e.,
    /// children do not get deleted on deletion of the parent!
    void add_child(object *child);

    /// Remove child #object from parent #node.
    /// @param child Child #object to be added to parent #node.
    /// @note The parent #node does not take ownership of the given child, i.e.,
    /// children do not get deleted on deletion of the parent!
    void remove_child(object *child);

    object *find_child(const std::string &uri, char separator = ':');

    /// The same as value().
    string_seq children() const;

    /// Returns an interator for the list of children
    iterator begin();

    /// Returns an interator for the list of children
    iterator end();

private:
    /// Finds first child #object with given name.
    /// @param name Child #object name.
    /// @return Pointer to found child #object or nullptr.
    object *find_immediate_child(const std::string &name);

    // We use std::list because iterators of a list remain valid when elements
    // are deleted.
    children_t children_;
};

} // namespace decof

#endif // DECOF_NODE_H
