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

#ifndef DECOF_CLIENT_CONTEXT_H
#define DECOF_CLIENT_CONTEXT_H

#include <map>
#include <memory>

#include <boost/any.hpp>
#include <boost/signals2.hpp>

#include "client_read_interface.h"
#include "userlevel.h"

namespace decof
{

class connection;
class object;
class object_dictionary;
class object_visitor;

class client_context : public std::enable_shared_from_this<client_context>
{
public:
    explicit client_context(object_dictionary& a_object_dictionary, std::shared_ptr<connection> connection, userlevel_t userlevel = Normal);
    virtual ~client_context();

    userlevel_t userlevel() const;
    void userlevel(userlevel_t ul);

    /// Returns the connection type, e.g., "tcp", "serial".
    virtual std::string connection_type() const = 0;

    /// @brief Returns the connections' remote endpoint.
    /// For a TCP connection this results in a string such as "10.0.0.1:1234".
    virtual std::string remote_endpoint() const = 0;

    virtual void preload() = 0;

protected:
    void set_parameter(const std::string& uri, const boost::any& any_value, char separator = ':');
    boost::any get_parameter(const std::string& uri, char separator = ':');

    /// Signal event.
    /// @param uri The event URI.
    /// @param separator Hierachy level separator charater used in URI.
    void signal_event(const std::string &uri, char separator = ':');

    void observe(const std::string& uri, client_read_interface::signal_type::slot_type slot);
    void unobserve(const std::string& uri);

    /** @brief Traverse object tree using visitor pattern.
     * @param visitor Pointer to visitor object.
     * @param root_uri Root element to start browsing. If omitted browsing
     * starts at root uri. */
    void browse(object_visitor *visitor, const std::string &root_uri = std::string());

    object_dictionary& object_dictionary_;
    std::shared_ptr<connection> connection_;

private:
    void browse_object(object *te, object_visitor *visitor);

    userlevel_t userlevel_;
    std::map<std::string, boost::signals2::connection> observables_;
};

} // namespace decof

#endif // DECOF_CLIENT_CONTEXT_H
