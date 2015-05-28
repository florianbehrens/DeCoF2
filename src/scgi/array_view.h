/*
 * Copyright (c) 2015 Florian Behrens
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

#include <cstddef>

namespace decof
{

namespace scgi
{

/*** @brief Static array container without memory ownership.
 * This container provides the standard container interface but does not hold
 * ownership of the underlying memory. */
template<typename T>
struct array_view
{
    typedef T value_type;
    typedef T* iterator;
    typedef const T* const_iterator;

    /// Constructs #array_view from pointer with size.
    explicit array_view(T *const ptr, std::size_t size) :
        ptr_(ptr),
        size_(size)
    {}

    /// Constructs #array_view from begin and end pointer.
    explicit array_view(T *const begin, T *const end) :
        ptr_(begin),
        size_(end - begin)
    {
        assert(end >= begin);
    }

    T& at(size_t pos) {
        if (pos >= size_)
            throw std::range_error("array_view: out of range access");
        return ptr_[pos];
    }

    const T& at(size_t pos) const {
        if (pos >= size_)
            throw std::range_error("array_view: out of range access");
        return ptr_[pos];
    }

    T& operator=(size_t pos) {
        return ptr_[pos];
    }

    const T& operator=(size_t pos) const {
        return ptr_[pos];
    }

    T& front() {
        return ptr_[0];
    }

    const T& front() const {
        return ptr_[0];
    }

    T& back() {
        return ptr_[size_-1];
    }

    const T& back() const {
        return ptr_[size_-1];
    }

    T* data() {
        return ptr_;
    }

    const T* data() const {
        return ptr_;
    }

    iterator begin() {
        return ptr_;
    }

    const_iterator begin() const {
        return ptr_;
    }

    const_iterator cbegin() const {
        return ptr_;
    }

    iterator end() {
        return ptr_ + size_;
    }

    const_iterator end() const {
        return ptr_ + size_;
    }

    const_iterator cend() const {
        return ptr_ + size_;
    }

    size_t size() const {
        return size_;
    }

private:
    T *const ptr_;
    const std::size_t size_;
};

} // namespace decof

} // namespace scgi
