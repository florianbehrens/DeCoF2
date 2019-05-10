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

#ifndef DECOF_OBJECT_H
#define DECOF_OBJECT_H

#include <string>
#include "automatic_ptr.h"
#include "userlevel.h"

namespace decof
{

class node;
class object_dictionary;
class object_visitor;

/** @brief Abstract object class.
 * This is the base class of all objects in a DeCoF object tree. All objects
 * have in common a short (#name) and fully qualified name (#fq_name), a parent
 * (#parent, except the root object), and read (#readlevel) and write levels
 * (#writelevel). */
class object
{
    friend class node;

public:
    object(std::string name, node *parent, userlevel_t readlevel, userlevel_t writelevel);
    virtual ~object();

    object(object&) = delete;
    object &operator=(object &) = delete;

    std::string name() const;
    void name(std::string name);

    std::string fq_name() const;

    node *parent() const;
    void reset_parent(node *parent = nullptr);

    userlevel_t readlevel() const;
    void readlevel(userlevel_t readlevel);

    userlevel_t writelevel() const;
    void writelevel(userlevel_t writelevel);

    /// Visitor pattern accept method
    virtual void accept(object_visitor *visitor);

    /// Returns a pointer to the object dictionary of this tree element or
    /// nullptr if not defined.
    object_dictionary* get_object_dictionary();

    /// Returns a constant pointer to the object dictionary of this tree
    /// element or nullptr if not defined.
    const object_dictionary* get_object_dictionary() const;

private:
    std::string name_;
    automatic_ptr<node> parent_;
    userlevel_t readlevel_;
    userlevel_t writelevel_;
};

} // namespace decof

#endif // DECOF_OBJECT_H
