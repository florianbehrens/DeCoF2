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

namespace decof
{

namespace cli
{

class object;

class tree_visitor : public decof::object_visitor
{
public:
    explicit tree_visitor(std::ostream &out);

    virtual void visit(decof::event *event) override;
    virtual void visit(decof::node *node) override;
    virtual void visit(decof::object *obj) override;
    virtual void visit(decof::basic_parameter<boolean> *param) override;
    virtual void visit(decof::basic_parameter<integer> *param) override;
    virtual void visit(decof::basic_parameter<real> *param) override;
    virtual void visit(decof::basic_parameter<string> *param) override;
    virtual void visit(decof::basic_parameter<binary> *param) override;
    virtual void visit(decof::basic_parameter<boolean_seq> *param) override;
    virtual void visit(decof::basic_parameter<integer_seq> *param) override;
    virtual void visit(decof::basic_parameter<real_seq> *param) override;
    virtual void visit(decof::basic_parameter<string_seq> *param) override;
    virtual void visit(decof::basic_parameter<binary_seq> *param) override;

private:
    void write_param(decof::object *obj, const char *type);

    std::ostream &out_;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_TREE_VISITOR_H
