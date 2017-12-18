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

#ifndef DECOF_CLI_TREE_VISITOR_H
#define DECOF_CLI_TREE_VISITOR_H

#include <ostream>

#include <decof/client_context/object_visitor.h>
#include <decof/types.h>

namespace decof
{

class object;
class event;
class node;

namespace cli
{

class tree_visitor : public object_visitor
{
public:
    explicit tree_visitor(std::ostream &out);

    virtual void visit(object* obj) override;
    virtual void visit(event *event) override;
    virtual void visit(node *node) override;

    virtual void visit(object* obj, boolean_tag) override;
    virtual void visit(object* obj, integer_tag) override;
    virtual void visit(object* obj, real_tag) override;
    virtual void visit(object* obj, string_tag) override;
    virtual void visit(object* obj, binary_tag) override;

    virtual void visit(object* obj, sequence_tag<boolean_tag>) override;
    virtual void visit(object* obj, sequence_tag<integer_tag>) override;
    virtual void visit(object* obj, sequence_tag<real_tag>) override;
    virtual void visit(object* obj, sequence_tag<string_tag>) override;

    virtual void visit(object* obj, tuple_tag) override;

private:
    void write_param(decof::object *obj, const char *type);

    std::ostream &out_;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_TREE_VISITOR_H
