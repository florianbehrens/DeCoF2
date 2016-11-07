/*
 * Copyright (c) 2016 Florian Behrens
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

#ifndef DECOF_CLI_CONTEXT_BASE_H
#define DECOF_CLI_CONTEXT_BASE_H

#include <functional>
#include <string>

#include <decof/userlevel.h>
#include <decof/client_context/client_context.h>

namespace decof
{

namespace cli
{

class cli_context_base : public client_context
{
public:
    using userlevel_cb_t = std::function<bool(const client_context&, userlevel_t, const std::string&)>;

    using client_context::client_context;

    /** @brief Call to install a userlevel change callback.
     *
     * The given callable is invoked each time the userlevel is changed. You
     * can deny this operation by returning #false.
     *
     * @param userlevel_cb The callable to be invoked on userlevel changes. */
    static void install_userlevel_callback(const userlevel_cb_t& userlevel_cb) noexcept;

protected:
    static userlevel_cb_t userlevel_cb_;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_CONTEXT_BASE_H
