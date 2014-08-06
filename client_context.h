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

#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <memory>

#include "client_access.h"

namespace decof
{

class connection;
class object_dictionary;

class client_context : public client_access
{
    typedef int userlevel_t;

public:
    explicit client_context(object_dictionary& a_object_dictionary, connection* a_connection);
    virtual ~client_context();

    userlevel_t userlevel() const;
    void set_userlevel(userlevel_t a_userlevel);

    connection const* connnection() const;

    virtual void preload() = 0;

private:
    void set_parameter(const std::string& uri, const boost::any& value) override;
    boost::any get_parameter(const std::string& uri) override;

    object_dictionary& object_dictionary_;
    std::unique_ptr<connection> connection_;
};

} // namespace decof

#endif // CLIENT_CONTEXT_H
