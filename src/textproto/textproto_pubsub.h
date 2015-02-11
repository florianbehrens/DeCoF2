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

#ifndef TEXTPROTO_PUBSUB_H
#define TEXTPROTO_PUBSUB_H

#include "client_context.h"

#include <string>

#include <boost/any.hpp>

namespace decof
{

class textproto_pubsub : public client_context
{
public:
    // We inherit base class constructors
    using client_context::client_context;

    virtual std::string connection_type() const override final;
    virtual std::string remote_endpoint() const override final;
    virtual void preload() override final;

private:
    void read_handler(const std::string &cstr);
    void notify(const std::string &uri, const boost::any &any_value);
};

} // namespace decof

#endif // TEXTPROTO_PUBSUB_H
