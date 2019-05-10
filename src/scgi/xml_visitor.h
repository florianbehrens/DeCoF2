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

#include <decof/object_visitor.h>
#include <decof/userlevel.h>
#include <ostream>
#include <stack>

namespace decof {

namespace scgi {

/**
 * @brief Generator of XML parameter tree description.
 */
class xml_visitor : public object_visitor
{
  public:
    /**
     * Constructor.
     *
     * @param out Stream to write generated data to.
     * @param system_name The name of the XML root node.
     */
    explicit xml_visitor(std::ostream& out, const std::string& system_name = "DeCoF2 server");
    virtual ~xml_visitor();

    virtual void visit(event* event) override;
    virtual void visit(node* node) override;

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
    void write_param(const object* obj, const std::string& type_str);

    std::ostream&     out_;
    std::stack<node*> node_stack_;
    bool              first_pass_ = true;
};

} // namespace scgi

} // namespace decof

#endif // DECOF_SCGI_XML_VISITOR_H
