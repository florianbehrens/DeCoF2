/*
 * Copyright (c) 2014 Florian Behrens
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

#ifndef DECOF_AUTOMATIC_PTR_H
#define DECOF_AUTOMATIC_PTR_H

#include <memory>

namespace decof
{

template<typename T>
class automatic_ptr;

/** @brief Base class for target objects used with #automatic_ptr.
 *
 * @tparam T The target object type, i.e., the class that derives from
 * #automatic_ptr_target.
 */
template<typename T>
class automatic_ptr_target
{
    friend class automatic_ptr<T>;

    template<typename U>
    struct basic_null_deleter
    {
        void operator()(U*)
        {}
    };

    using null_deleter = basic_null_deleter<T>;

protected:
    automatic_ptr_target() :
        ptr_(static_cast<T*>(this), null_deleter())
    {}

    virtual ~automatic_ptr_target() = default;

    automatic_ptr_target(const automatic_ptr_target&) :
        ptr_(static_cast<T*>(this), null_deleter())
    {}

    automatic_ptr_target(automatic_ptr_target&& rhs) :
        ptr_(static_cast<T*>(this), null_deleter())
    {
        rhs.ptr_.reset();
    }

    automatic_ptr_target& operator=(const automatic_ptr_target&)
    {
        return *this;
    }

    automatic_ptr_target& operator=(automatic_ptr_target&& rhs)
    {
        rhs.ptr_.reset();
        return *this;
    }

private:
    std::shared_ptr<T> ptr_;
};

/** @brief An intrusive smart pointer type that is reset automatically on
 * target object destruction.
 *
 * This pointer requires the target type be derived from #automatic_ptr_target.
 */
template<typename T>
class automatic_ptr final
{
public:
    /// Default construction.
    /// @post #get() returns nullptr, #operator bool returns false.
    automatic_ptr() noexcept = default;
    automatic_ptr(const automatic_ptr&) noexcept = default;

    explicit automatic_ptr(const automatic_ptr_target<T>* target) noexcept :
        ptr_(target ? target->ptr_ : nullptr)
    {}

    automatic_ptr& operator=(const automatic_ptr& rhs) noexcept = default;
    automatic_ptr& operator=(const automatic_ptr_target<T>* target) noexcept {
        ptr_ = target ? target->ptr_ : nullptr;
        return *this;
    }

    /// Returns whether the target object is alive.
    operator bool() const noexcept {
        return !ptr_.expired();
    }

    /// Returns the stored pointer.
    T* get() const noexcept {
        return ptr_.lock().get();
    }

    /// Dereferences the stored pointer.
    T* operator->() const noexcept {
        return get();
    }

    /// Dereferences the stored pointer.
    T& operator*() const noexcept {
        return *get();
    }

    /// Resets the stored pointer.
    /// @post #get() returns nullptr.
    void reset() noexcept {
        ptr_.reset();
    }

private:
    std::weak_ptr<T> ptr_;
};

} // namespace decof

#endif // DECOF_AUTOMATIC_PTR_H
