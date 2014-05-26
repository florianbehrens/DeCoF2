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

#ifndef TREE_ELEMENT_H
#define TREE_ELEMENT_H

#include <string>

#include <boost/signals2.hpp>

// Forward declarations
namespace boost {
class any;
}

class observer;
class node;

/** Abstract tree element class.
 *
 * @pattern Compositum.
 */
class tree_element
{
protected:
    explicit tree_element(std::string name, node *parent = nullptr);

public:
    typedef boost::signals2::signal<void (const std::string&, const boost::any&)> signal_type;
    typedef signal_type::slot_type slot_type;
    typedef boost::signals2::connection connection;

    virtual ~tree_element();

    std::string name() const;
    void set_name(std::string name);

    std::string fq_name() const;

    node *parent() const;
    void set_parent(node *parent);

    // Provides the value as runtime-generic type.
    virtual boost::any any_value() noexcept = 0;

    // Observe parameter value.
    virtual connection observe(slot_type slot) noexcept = 0;

private:
    std::string name_;
    node *parent_;
};

#endif // TREE_ELEMENT_H
