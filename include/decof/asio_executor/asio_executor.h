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

#ifndef DECOF_ASIO_EXECUTOR_H
#define DECOF_ASIO_EXECUTOR_H

#include <type_traits>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/thread/sync_queue.hpp>

namespace decof {

/**
 * @brief Executor for Boost.Asio IO service.
 *
 * This class models the Executor concept as specified in
 * <a href="http://www.boost.org/doc/libs/1_64_0/doc/html/thread/synchronization.html#thread.synchronization.executors.ref.concept_executor">
 * Boost.Thread</a>.
 *
 * It can be used to asynchonously schedule work on the given
 * @c boost::asio::strand object using @c boost::async or
 * @c boost::future::then.
 */
class asio_executor : boost::noncopyable
{
    using strand = boost::asio::io_service::strand;

    /**
     * @brief Wrapper class to overcome Boost.Asio's requirement that
     * asynchronous handlers must be copy constructible.
     *
     * This is taken from
     * https://stackoverflow.com/questions/17211263/how-to-trick-boostasio-to-allow-move-only-handlers
     */
    template <typename F>
    struct move_wrapper : F
    {
        move_wrapper(F&& f) : F(std::move(f)) {}

        move_wrapper(move_wrapper&&) = default;
        move_wrapper& operator=(move_wrapper&&) = default;

        move_wrapper(const move_wrapper&);
        move_wrapper& operator=(const move_wrapper&);
    };

    /**
     * @brief Wrapper function to overcome Boost.Asio's requirement that
     * asynchronous handlers must be copy constructible.
     *
     * This is taken from
     * https://stackoverflow.com/questions/17211263/how-to-trick-boostasio-to-allow-move-only-handlers
     */
    template <typename T>
    auto move_handler(T&& t) -> move_wrapper<typename std::decay<T>::type>
    {
        return std::move(t);
    }

public:
    explicit asio_executor(strand& s);

    /**
     * @brief Submit a callable to the executor.
     *
     * The callable will be scheduled for execution at some point in the future.
     * If invoked callable throws an exception the executor will call @c
     * std::terminate, as is the case with threads.
     *
     * The callable must have the signature @c void(). If you want to submit a
     * callable with arguments, wrap it in a lambda or use @c std::bind. If you
     * want to submit a callable with a return value, @c std::packaged_task is
     * your friend.
     *
     * @param callable The callable to submit.
     */
    template<typename F>
    void submit(F&& callable)
    {
        if (closed_)
            throw boost::sync_queue_is_closed();

        strand_.post(move_handler(callable));
    }

    void close();
    bool closed();
    bool try_executing_one();

private:
    bool closed_{ false };
    strand strand_;
};

} // namespace decof

#endif // DECOF_ASIO_EXECUTOR_H
