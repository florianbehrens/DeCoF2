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

#include <decof/cli/cli_context_base.h>

namespace decof
{

namespace cli
{

cli_context_base::userlevel_cb_t cli_context_base::userlevel_cb_ =
    [](const client_context&, userlevel_t, const std::string&) { return false; };

void cli_context_base::install_userlevel_callback(const cli_context_base::userlevel_cb_t &userlevel_cb) noexcept
{
    userlevel_cb_ = userlevel_cb;
}

} // namespace cli

} // namespace decof
