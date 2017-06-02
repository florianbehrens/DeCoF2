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

#include "background_worker.h"

#include <chrono>
#include <thread>

#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION
#define BOOST_THREAD_PROVIDES_EXECUTORS
#include <boost/thread/future.hpp>

#include "application.h"

background_worker::ptr background_worker::create(const std::string& name, decof::node* parent)
{
    background_worker::ptr self(new background_worker(name, parent));
    return self;
}

background_worker::background_worker(const std::string& name, decof::node* parent) :
    decof::node(name, parent),
    event_("start", this, std::bind(&background_worker::start, this)),
    ready_("ready", this, false)
{}

void background_worker::start()
{
    auto self = shared_from_this();

    ready_.value(false);

    boost::async([self]() {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }).then(
        application::instance().executor(),
        [self](boost::unique_future<void>) {
            self->ready_.value(true);
    });
}
