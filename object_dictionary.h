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

#include "node.h"

namespace decof
{

// Forward declaration
class Server;

/// @brief Object dictionary for parameter tree objects.
class ObjectDictionary : public node
{
public:
    friend class Server;

    tree_element* find_object(std::string uri);

private:
    ObjectDictionary();
};

} // namespace decof

#endif // OBJECT_DICTIONARY_H
