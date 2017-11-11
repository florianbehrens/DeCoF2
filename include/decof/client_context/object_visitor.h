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

#include <sstream>

#include <decof/types.h>

namespace decof
{

// Forward declarations
class client_read_interface;
class client_write_interface;
class event;
class node;
template<typename T>
class basic_parameter;
class object;

/** @brief Abstract object dictionary visitor class.
 * See the <a href="http://en.wikipedia.org/wiki/Visitor_pattern">Visitor</a>
 * pattern. */
class object_visitor
{
public:
    virtual void visit(event *event);
    virtual void visit(node *node);
//    virtual void visit(object*) = 0;
    virtual void visit(client_read_interface* param);
    virtual void visit(client_write_interface* param);

//    virtual void visit(basic_parameter<boolean> *param);
//    virtual void visit(basic_parameter<integer> *param);
//    virtual void visit(basic_parameter<real> *param);
//    virtual void visit(basic_parameter<string> *param);
//    virtual void visit(basic_parameter<binary> *param);
//    virtual void visit(basic_parameter<boolean_seq> *param);
//    virtual void visit(basic_parameter<integer_seq> *param);
//    virtual void visit(basic_parameter<real_seq> *param);
//    virtual void visit(basic_parameter<string_seq> *param);
//    virtual void visit(basic_parameter<binary_seq> *param);

protected:
    void write_indentation(std::stringstream &ss, const object *te);
    std::string indentation();

    void increment_indentation(size_t levels = 1u);
    void decrement_indentation(size_t levels = 1u);

    size_t indentation_baselevel_ = 0u;
};

} // namespace decof

#endif // DECOF_OBJECT_VISITOR_H
