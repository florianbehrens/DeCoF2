/*
 * Copyright (c) 2019 Florian Behrens
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

#ifndef DECOF_TRANSFORM_ITERATOR_H
#define DECOF_TRANSFORM_ITERATOR_H

#include <iterator>
#include <utility>

namespace decof {

template <typename Iterator, typename UnaryFunc>
class transform_iterator
{
  public:
    using difference_type = std::ptrdiff_t;
    using value_type = typename std::result_of<UnaryFunc(typename std::iterator_traits<Iterator>::value_type)>::type;
    using pointer    = value_type*;
    using reference  = value_type&;
    using iterator_category = std::input_iterator_tag;

    transform_iterator(Iterator it, UnaryFunc func) : it_(it), func_(func)
    {
    }

    transform_iterator(const transform_iterator& rhs) = default;
    ~transform_iterator()                             = default;

    transform_iterator<Iterator, UnaryFunc>& operator=(const transform_iterator<Iterator, UnaryFunc>& rhs) = default;

    void swap(transform_iterator<Iterator, UnaryFunc>& other)
    {
        std::swap(it_, other.it_);
        std::swap(func_, other.func_);
    }

    value_type operator*() const
    {
        return func_(*it_);
    }

    value_type operator->() const
    {
        return func_(it_.operator->());
    }

    transform_iterator<Iterator, UnaryFunc>& operator++()
    {
        ++it_;
        return *this;
    }

    transform_iterator<Iterator, UnaryFunc> operator++(int)
    {
        transform_iterator<Iterator, UnaryFunc> tmp(*this);
        ++it_;
        return tmp;
    }

    bool operator==(const transform_iterator<Iterator, UnaryFunc>& rhs)
    {
        return it_ == rhs.it_ && func_ == rhs.func_;
    }

    bool operator!=(const transform_iterator<Iterator, UnaryFunc>& rhs)
    {
        return !(*this == rhs);
    }

  private:
    Iterator  it_;
    UnaryFunc func_;
};

} // namespace decof

namespace std {

template <typename Iterator, typename UnaryFunc>
struct iterator_traits<decof::transform_iterator<Iterator, UnaryFunc>>
{
    using difference_type   = typename decof::transform_iterator<Iterator, UnaryFunc>::difference_type;
    using value_type        = typename decof::transform_iterator<Iterator, UnaryFunc>::value_type;
    using pointer           = typename decof::transform_iterator<Iterator, UnaryFunc>::pointer;
    using reference         = typename decof::transform_iterator<Iterator, UnaryFunc>::reference;
    using iterator_category = typename decof::transform_iterator<Iterator, UnaryFunc>::iterator_category;
};

} // namespace std

#endif // DECOF_TRANSFORM_ITERATOR_H
