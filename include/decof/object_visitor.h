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

#ifndef DECOF_OBJECT_VISITOR_H
#define DECOF_OBJECT_VISITOR_H

#include <decof/types.h>
#include <ostream>

namespace decof {

// Forward declarations
class event;
class node;
class object;

/** @brief Abstract object dictionary visitor class.
 * See the <a href="http://en.wikipedia.org/wiki/Visitor_pattern">Visitor</a>
 * pattern. */
class object_visitor
{
  public:
    virtual ~object_visitor() = default;

    /**
     * @name Language type visitor functions.
     * @{
     */
    virtual void visit(object*)
    {
    }
    virtual void visit(event* e);
    virtual void visit(node* n);
    ///@}

    /**
     * @name Parameter value type class visitor functions.
     * @{
     */
    virtual void visit(object* obj, boolean_tag);
    virtual void visit(object* obj, integer_tag);
    virtual void visit(object* obj, real_tag);
    virtual void visit(object* obj, string_tag);
    virtual void visit(object* obj, binary_tag);

    virtual void visit(object* obj, sequence_tag<boolean_tag>);
    virtual void visit(object* obj, sequence_tag<integer_tag>);
    virtual void visit(object* obj, sequence_tag<real_tag>);
    virtual void visit(object* obj, sequence_tag<string_tag>);

    virtual void visit(object* obj, tuple_tag);
    ///@}

  protected:
    void        write_indentation(std::ostream& out, const object* te);
    std::string indentation();

    void increment_indentation(size_t levels = 1u);
    void decrement_indentation(size_t levels = 1u);

    size_t indentation_baselevel_ = 0u;
};

} // namespace decof

#endif // DECOF_OBJECT_VISITOR_H
