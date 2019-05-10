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

#include <decof/cli/update_container.h>
#include <cassert>

namespace decof
{

namespace cli
{

update_container::update_container() :
    front_(updates_.end()),
    back_(updates_.end())
{}

void update_container::push(const key_type& uri, const value_t& value)
{
    // Make sure iterators are either both invalid or both valid.
    assert((front_ == updates_.end() && back_ == updates_.end()) ||
           (front_ != updates_.end() && back_ != updates_.end()));

    bool is_new;
    container_type::iterator current;

    std::tie(current, is_new) = updates_.emplace(uri, container_type::mapped_type());

    current->second.value = value;
    current->second.time = std::chrono::system_clock::now();

    if (is_new) {
        current->second.next = updates_.end();

        if (front_ == updates_.end()) {
            // First element: Initialize iterators
            back_ = current;
            front_ = current;
        } else {
            // Additional element: Update next pointer of previous element
            // and let back_ iterator point to tail.
            back_->second.next = current;
            back_ = current;
        }
    }
}

std::tuple<update_container::key_type, value_t, update_container::time_point> update_container::pop_front()
{
    if (front_ == updates_.end())
        throw std::out_of_range("Container empty");

    auto retval = std::make_tuple(std::move(front_->first), std::move(front_->second.value), std::move(front_->second.time));

    auto it = front_;
    front_ = it->second.next;
    updates_.erase(it);
    if (front_ == updates_.end())
        back_ = updates_.end();

    // Make sure iterators are either both invalid or both valid.
    assert((front_ == updates_.end() && back_ == updates_.end()) ||
           (front_ != updates_.end() && back_ != updates_.end()));

    return retval;
}

bool update_container::empty() const noexcept
{
    return updates_.empty();
}

} // namespace cli

} // namespace decof
