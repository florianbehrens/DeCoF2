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

#ifndef DECOF_CLI_BROWSE_VISITOR_H
#define DECOF_CLI_BROWSE_VISITOR_H

#include <ostream>
#include <decof/client_context/object_visitor.h>

namespace decof
{

class node;
class client_read_interface;

namespace cli
{

class browse_visitor : public object_visitor
{
public:
    explicit browse_visitor(std::ostream &out);

    virtual void visit(object* obj) override;
    virtual void visit(node* node) override;

private:
    std::ostream &out_;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_BROWSE_VISITOR_H
