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

#ifndef DECOF_SCGI_XML_VISITOR_H
#define DECOF_SCGI_XML_VISITOR_H

#include <stack>
#include <ostream>

#include <decof/client_context/object_visitor.h>
#include <decof/userlevel.h>

namespace decof
{

namespace scgi
{

class xml_visitor : public object_visitor
{
public:
    explicit xml_visitor(std::ostream &out);
    virtual ~xml_visitor();

    void visit(event* event) override;
    void visit(node* node) override;

    virtual void visit(object* obj, boolean);
    virtual void visit(object* obj, integer);
    virtual void visit(object* obj, real);
    virtual void visit(object* obj, string_t);
    virtual void visit(object* obj, binary_t);

    virtual void visit(object* obj, sequence<boolean>);
    virtual void visit(object* obj, sequence<integer>);
    virtual void visit(object* obj, sequence<real>);
    virtual void visit(object* obj, sequence<string_t>);
    virtual void visit(object* obj, sequence<binary_t>);

    virtual void visit(object* obj, tuple_t);

private:
    void write_param(const object *obj, const std::string &type_str);

    std::ostream &out_;
    std::stack<node*> node_stack_;
    bool first_pass_ = true;
};

} // namespace scgi

} // namespace decof

#endif // DECOF_SCGI_XML_VISITOR_H
