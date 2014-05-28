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

#include <boost/algorithm/string/predicate.hpp>

ObjectDictionary::ObjectDictionary()
 : node("root", nullptr)
{}

tree_element *ObjectDictionary::find_object(std::string uri)
{
    if (uri == name())
        return this;

    if (boost::algorithm::starts_with(uri, name() + ":")) {
        std::string sub_uri = uri.substr(uri.find(':') + 1);
        return find_child(sub_uri);
    }

    return nullptr;
}
