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

#ifndef DECOF_JSON_VISITOR_H
#define DECOF_JSON_VISITOR_H

#include "object_visitor.h"

#include <functional>
#include <memory>
#include <stack>

namespace decof
{

class json_visitor : public object_visitor
{
public:
    explicit json_visitor(std::stringstream &ss);
    virtual ~json_visitor();

    virtual void visit(event *ev) override;
    virtual void visit(node *node) override;
    virtual void visit(basic_parameter *param) override;
    virtual void visit(typed_parameter<boolean> *param) override;
    virtual void visit(typed_parameter<integer> *param) override;
    virtual void visit(typed_parameter<real> *param) override;
    virtual void visit(typed_parameter<string> *param) override;
    virtual void visit(typed_parameter<binary> *param) override;
    virtual void visit(typed_parameter<boolean_seq> *param) override;
    virtual void visit(typed_parameter<integer_seq> *param) override;
    virtual void visit(typed_parameter<real_seq> *param) override;
    virtual void visit(typed_parameter<string_seq> *param) override;
    virtual void visit(typed_parameter<binary_seq> *param) override;

private:
    void write_param(basic_parameter *param, const std::string &type_str);
    void check_indentation(object *obj);

    std::stringstream &ss_;
    node *current_node_;
};

} // namespace decof

#endif // DECOF_JSON_VISITOR_H
