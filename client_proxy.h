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

#ifndef CLIENT_PROXY_H
#define CLIENT_PROXY_H

#include <map>
#include <string>

#include "errors.h"
#include "object_dictionary.h"
#include "readwrite_parameter.h"
#include "server.h"

class tree_element;

class ClientProxy
{
protected:
    explicit ClientProxy(Server& server);

    /// @brief Virtual destructor.
    /// Makes sure that active connections are properly disconnected.
    virtual ~ClientProxy();

public:
    virtual void preload() = 0;

protected:
    template<typename T>
    void set_parameter(std::string uri, T value) {
        if (tree_element *te = server_.objectDictionary().find_child(uri)) {
            // Check parameter type
            basic_parameter<T> *bp = dynamic_cast<basic_parameter<T>*>(te);

            if (bp != nullptr) {
                // Check if parameter writable
                readwrite_parameter<T> *rwp = dynamic_cast<readwrite_parameter<T>*>(bp);

                if (rwp != nullptr) {
                    rwp->set_private_value(value);
                } else
                    throw access_denied_error();
            } else
                throw wrong_type_error();
        } else
            throw invalid_parameter_error();
    }

public:
    void observe(std::string uri, tree_element::signal_type::slot_type slot);
    void unobserve(std::string uri);

protected:
    Server& server_;

private:
    std::map<std::string, boost::signals2::connection> observables_;
};

#endif // CLIENT_PROXY_H
