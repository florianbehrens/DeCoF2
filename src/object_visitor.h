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

#include "types.h"

namespace decof
{

// Forward declarations
class basic_parameter;
class event;
class node;
template<typename T>
class observable_parameter;

/// Abstract object dictionary visitor class.
/// @pattern Visitor
class object_visitor
{
public:
    virtual void visit(event*);
    virtual void visit(node*);
    virtual void visit(basic_parameter*) = 0;
    virtual void visit(observable_parameter<boolean> *param);
    virtual void visit(observable_parameter<integer> *param);
    virtual void visit(observable_parameter<real> *param);
    virtual void visit(observable_parameter<string> *param);
    virtual void visit(observable_parameter<binary> *param);
    virtual void visit(observable_parameter<boolean_seq> *param);
    virtual void visit(observable_parameter<integer_seq> *param);
    virtual void visit(observable_parameter<real_seq> *param);
    virtual void visit(observable_parameter<string_seq> *param);
    virtual void visit(observable_parameter<binary_seq> *param);
};

} // namespace decof

#endif // DECOF_OBJECT_VISITOR_H
