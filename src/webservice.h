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

#ifndef DECOF_WEBSERVICE_H
#define DECOF_WEBSERVICE_H

#include <string>

#include "client_context.h"
#include "request_parser.hpp"

namespace decof
{

class webservice : public client_context
{
public:
    // We inherit base class constructors
    using client_context::client_context;

    virtual void preload() override final;

private:
    void read_handler(const std::string &cstr);
    void write_handler();
    void disconnect_handler();

    http::server::request_parser request_parser_;
};

} // namespace decof

#endif // DECOF_WEBSERVICE_H
