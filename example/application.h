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

#ifndef DECOF_EXAMPLE_APPLICATION_H
#define DECOF_EXAMPLE_APPLICATION_H

#include <memory>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <decof/asio_executor/asio_executor.h>

/**
 * @brief Singleton class representing the application global state.
 */
class application
{
public:
    static application& instance();

    /**
     * @brief Blocking applications main function.
     *
     * Call @c application::stop() to make the run function return.
     */
    int run(int, char*[]);

    /**
     * @brief Stop application.
     * @note This function is thread-safe.
     */
    void stop();

    boost::asio::io_service::strand& strand();
    decof::asio_executor& executor();

private:
    explicit application();

    boost::asio::io_service io_service_;
    boost::asio::io_service::strand strand_;
    decof::asio_executor executor_;
};

#endif // DECOF_EXAMPLE_APPLICATION_H
