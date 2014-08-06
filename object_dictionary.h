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

#ifndef OBJECT_DICTIONARY_H
#define OBJECT_DICTIONARY_H

#include <boost/asio.hpp>

#include "node.h"
#include "regular_timer.h"

namespace decof
{

class client_context;
class connection;

/// @brief Object dictionary for parameter tree objects.
class object_dictionary : public node
{
    friend class client_context;

public:
    object_dictionary();

    tree_element* find_object(std::string uri);

    regular_timer& get_fast_timer();
    regular_timer& get_medium_timer();
    regular_timer& get_slow_timer();

    boost::asio::io_service& get_io_service();

    void add_context(client_context* a_client_context);
    const client_context* current_context() const;
    void delete_current_context();

private:
    void set_current_context(client_context* a_client_context);

    boost::asio::io_service io_service_;
    regular_timer fast_timer_;
    regular_timer medium_timer_;
    regular_timer slow_timer_;

    std::list<std::unique_ptr<client_context>> client_contexts_;
};

} // namespace decof

#endif // OBJECT_DICTIONARY_H
