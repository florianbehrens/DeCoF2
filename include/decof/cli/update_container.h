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

#ifndef DECOF_CLI_UPDATE_CONTAINER_H
#define DECOF_CLI_UPDATE_CONTAINER_H

#include <chrono>
#include <string>
#include <tuple>
#include <map>

#include <decof/types.h>

namespace decof
{

namespace cli
{

/** Container for ordered storage of publish updates of pubsub pattern.
 */
class update_container {
public:
    typedef std::chrono::time_point<std::chrono::system_clock> time_point;
    typedef std::string key_type;
    typedef std::out_of_range out_of_range;

    update_container();

    update_container(update_container&) = delete;
    void operator=(update_container&) = delete;

    /** @brief Push new element to container.
     *
     * If @c uri does not already exist in the container, @c gen_value is
     * pushed to the end of the sequence. Otherwise the existing element is
     * replaced by the new one in sequence.
     *
     * @param uri The parameter URI.
     * @param value The value to be published. */
    void push(const key_type& uri, const value_t& value);

    /** Pop and return first element from container along with the time it was
     * pushed.
     * @return Tuple containing first element and timestamp.
     * @throw out_of_range if container is empty. */
    std::tuple<key_type, value_t, time_point> pop_front();

    bool empty() const noexcept;

private:
    template<
        template <typename K, typename V, typename C, typename A> class Map,
        typename Key
    >
    struct mapped_type {
    public:
        typedef typename Map<const Key, mapped_type, std::less<Key>, std::allocator<int>>::iterator iterator;

        time_point time;
        value_t value;
        iterator next;
    };

    typedef std::map<key_type, mapped_type<std::map, key_type>> container_type;
    container_type updates_;

    /// Remember iterator to oldest element in #updates_.
    /// @note Only call methods of #container_type that don't invalidate
    /// iterators like #container_type::emplace, #container_type::erase.
    container_type::iterator front_;

    /// Remember iterator to latest element in #updates_.
    /// @note See note of #front_.
    container_type::iterator back_;
};

} // namespace cli

} // namespace decof

#endif // DECOF_CLI_UPDATE_CONTAINER_H
