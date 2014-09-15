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

#ifndef DECOF_XML_VISITOR_H
#define DECOF_XML_VISITOR_H

#include "object_visitor.h"

#include <stack>

namespace decof
{

class xml_visitor : public object_visitor
{
public:
    explicit xml_visitor(std::stringstream &ss);
    virtual ~xml_visitor();

    virtual void visit(event *ev) override;
    virtual void visit(node *node) override;
    virtual void visit(basic_parameter*) override;
    virtual void visit(observable_parameter<boolean> *param) override;
    virtual void visit(observable_parameter<integer> *param) override;
    virtual void visit(observable_parameter<real> *param) override;
    virtual void visit(observable_parameter<string> *param) override;
    virtual void visit(observable_parameter<binary> *param) override;
    virtual void visit(observable_parameter<boolean_seq> *param) override;
    virtual void visit(observable_parameter<integer_seq> *param) override;
    virtual void visit(observable_parameter<real_seq> *param) override;
    virtual void visit(observable_parameter<string_seq> *param) override;
    virtual void visit(observable_parameter<binary_seq> *param) override;

private:
    void write_param(basic_parameter *param, const std::string &type_str);

    std::stringstream &ss_;
    std::stack<node*> node_stack_;
    bool first_pass_ = true;
};

} // namespace decof

#endif // DECOF_XML_VISITOR_H
