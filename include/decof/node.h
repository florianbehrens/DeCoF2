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

#include "automatic_ptr.h"
#include "observable_parameter.h"
#include "types.h"
#include <list>
#include <memory>
#include <string>
#include <string_view>

namespace decof {

class node : public automatic_ptr_target<node>, public observable_parameter<std::list<std::string>>
{
  public:
    typedef std::list<object*>   children_t;
    typedef children_t::iterator iterator;

  public:
    explicit node(std::string name, node* parent = nullptr, userlevel_t readlevel = Normal);
    virtual ~node();

    virtual std::list<std::string> value() const override;

    /// Visitor pattern accept method
    virtual void accept(object_visitor* visitor) override final;

    /**
     * @brief Add child object to node.
     *
     * @pre The child object must not have been added before. Adding the same child
     * twice is considered an error and results in undefined behavior.
     * @param child Child object to be added to parent node.
     * @note The parent node does not take ownership of the given child, i.e.,
     * children do not get deleted on deletion of the parent!
     */
    void add_child(object* child);

    /**
     * @brief Remove child object from parent node.
     *
     * @param child Child object to be added to parent node.
     * @note The parent node does not take ownership of the given child, i.e.,
     * children do not get deleted on deletion of the parent!
     */
    void remove_child(object* child);

    /**
     * @brief Find descendent object with given sub-URI.
     *
     * Returns a pointer to the object corresponding to the given sub-URI and
     * separator if existing, otherwise @c nullptr. The sub-URI must begin
     * with a node's child name rather than the name of the node itself.
     *
     * @param uri The sub-URI to the requested object.
     * @param separator The separator character used with the URI.
     * @return Pointer to the requested object if existing or @c nullptr.
     */
    object* find_descendant_object(std::string_view uri, char separator = ':');

    /// The same as value().
    std::list<std::string> children() const;

    /// Returns an iterator for the list of children.
    iterator begin();

    /// Returns an iterator for the list of children.
    iterator end();

  private:
    /**
     * @brief Finds first child object with given name.
     *
     * @param name Child object name.
     * @return Pointer to found child object or nullptr.
     */
    object* find_child(std::string_view name);

    // We use std::list because iterators of a list remain valid when elements
    // are deleted.
    children_t children_;
};

} // namespace decof

#endif // DECOF_NODE_H
