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

#ifndef DECOF_TREE_ELEMENT_H
#define DECOF_TREE_ELEMENT_H

#include <string>

#include <boost/signals2.hpp>

#include "userlevel.h"

// Forward declarations
namespace boost {
class any;
}

namespace decof
{

class object_dictionary;
class node;

/** Abstract tree element class.
 *
 * @pattern Compositum.
 */
class tree_element
{
protected:
    explicit tree_element(std::string name, node *parent, userlevel_t readlevel, userlevel_t writelevel);

public:
    typedef boost::signals2::signal<void (const std::string&, const boost::any&)> signal_type;
    typedef signal_type::slot_type slot_type;

    virtual ~tree_element();

    std::string name() const;
    void set_name(std::string name);

    std::string fq_name() const;

    node *parent() const;
    void set_parent(node *parent);

    userlevel_t readlevel() const;
    void readlevel(userlevel_t readlevel);

    userlevel_t writelevel() const;
    void writelevel(userlevel_t writelevel);

    /// Returns the object dictionary of this tree element or nullptr if not
    /// defined.
    object_dictionary* get_object_dictionary();

private:
    std::string name_;
    node *parent_;
    userlevel_t readlevel_;
    userlevel_t writelevel_;
};

} // namespace decof

#endif // DECOF_TREE_ELEMENT_H