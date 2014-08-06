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

#ifndef CLIENT_ACCESS_H
#define CLIENT_ACCESS_H

#include <boost/any.hpp>

namespace decof
{

class client_access
{
public:
    virtual void set_parameter(const std::string& uri, const boost::any& value) = 0;
    virtual boost::any get_parameter(const std::string& uri) = 0;
};

} // namespace decof

#endif // CLIENT_ACCESS_H
