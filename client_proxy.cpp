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

#include "client_proxy.h"

namespace decof
{

client_proxy::client_proxy(server &server)
  : server_(server)
{}

client_proxy::~client_proxy()
{
    // Disconnect all existing connections before destruction
    for (auto elem : observables_)
        elem.second.disconnect();
}

void client_proxy::observe(std::string uri, tree_element::signal_type::slot_type slot)
{
    if (tree_element *te = server_.objectDictionary().find_object(uri)) {
        if (observables_.count(uri) == 0) {
            tree_element::connection connection = te->observe(slot);
            observables_.emplace(uri, connection);
        }
    } else
        throw invalid_parameter_error();
}

void client_proxy::unobserve(std::string uri)
{
    try {
        observables_.at(uri).disconnect();
        observables_.erase(uri);
    } catch (std::out_of_range&) {
    }
}

} // namespace decof
