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

#ifndef DECOF_CLI_CLISRV_CONTEXT_H
#define DECOF_CLI_CLISRV_CONTEXT_H

#include "client_context.h"

namespace decof
{

namespace cli
{

class clisrv_context : public client_context
{
public:
    // We inherit base class constructors
    using client_context::client_context;

    virtual std::string connection_type() const override final;
    virtual std::string remote_endpoint() const override final;
    virtual void preload() override final;

private:
    /** @brief boost::asio read handler.
     * Parses and evaluates CLI client/server command lines. Expects
     * command lines like: <operation> [ <uri> [ <value-string> ]].
     * Operation must be one of: get, param-ref, set, param-set!, signal, exec,
     * browse, param-disp. */
    void read_handler(const std::string &cstr);

    void disconnect_handler();
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_CLISRV_CONTEXT_H
