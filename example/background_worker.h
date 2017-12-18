/*
 * Copyright (c) 2017 Florian Behrens
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

#ifndef DECOF_EXAMPLE_BACKGROUND_WORKER_H
#define DECOF_EXAMPLE_BACKGROUND_WORKER_H

#include <memory>
#include <thread>

#include <decof/handler_event.h>
#include <decof/managed_readonly_parameter.h>
#include <decof/node.h>

/**
 * @brief Background worker example.
 *
 * This class runs an exemplary task (in fact, just a
 * @c std::this_thread::sleep_for) in a background thread andupdates a parameter
 * as a result using a Boost.Thread executor.
 */
class background_worker final :
    public decof::node,
    public std::enable_shared_from_this<background_worker>
{
public:
    using ptr = std::shared_ptr<background_worker>;

    /// Factory method.
    static ptr create(const std::string& name, decof::node* parent = nullptr);

private:
    explicit background_worker(const std::string& name, decof::node* parent = nullptr);
    void start();

    decof::handler_event event_;
    decof::managed_readonly_parameter<bool> ready_;
};

#endif // DECOF_EXAMPLE_BACKGROUND_WORKER_H
