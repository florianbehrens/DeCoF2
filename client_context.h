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

#include <map>
#include <memory>

#include "connection.h"
#include "tree_element.h"

namespace decof
{

class connection;
class object_dictionary;

class client_context : public std::enable_shared_from_this<client_context>
{
    typedef int userlevel_t;

public:
    explicit client_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection);
    virtual ~client_context();

    userlevel_t userlevel() const;
    void set_userlevel(userlevel_t a_userlevel);

    connection const* connnection() const;

    virtual void preload() = 0;

protected:
    void set_parameter(const std::string& uri, const boost::any& any_value);
    boost::any get_parameter(const std::string& uri);

    void observe(const std::string& uri, tree_element::signal_type::slot_type slot);
    void unobserve(const std::string& uri);

    object_dictionary& object_dictionary_;
    std::shared_ptr<connection> connection_;

private:
    std::map<std::string, boost::signals2::connection> observables_;
};

} // namespace decof

#endif // CLIENT_CONTEXT_H
