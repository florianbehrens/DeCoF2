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

#include "object_dictionary.h"

#include <boost/algorithm/string.hpp>

#include "client_context.h"

namespace decof
{

object_dictionary::object_dictionary()
 : node("root", nullptr),
   fast_timer_(io_service_, std::chrono::milliseconds(50)),
   medium_timer_(io_service_, std::chrono::milliseconds(500)),
   slow_timer_(io_service_, std::chrono::milliseconds(5000))
{
     fast_timer_.start();
     medium_timer_.start();
     slow_timer_.start();
}

tree_element *object_dictionary::find_object(std::string uri)
{
    if (uri == name())
        return this;

    if (boost::algorithm::starts_with(uri, name() + ":")) {
        std::string sub_uri = uri.substr(uri.find(':') + 1);
        return find_child(sub_uri);
    }

    return nullptr;
}

regular_timer& object_dictionary::get_fast_timer()
{
    return fast_timer_;
}

regular_timer& object_dictionary::get_medium_timer()
{
    return medium_timer_;
}

regular_timer& object_dictionary::get_slow_timer()
{
    return slow_timer_;
}

boost::asio::io_service& object_dictionary::get_io_service()
{
    return io_service_;
}

void object_dictionary::add_context(client_context *a_client_context)
{}

const client_context *object_dictionary::current_context() const
{
    assert(false);
    return nullptr;
}

void object_dictionary::delete_current_context()
{}

} // namespace decof
