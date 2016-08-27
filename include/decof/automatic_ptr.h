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
#include <stdexcept>

namespace decof
{

template<typename T>
class automatic_ptr;

template<typename T>
struct control_block
{
    T* target;
};

template<typename T>
class automatic_ptr_target
{
    friend class automatic_ptr<T>;

protected:
    automatic_ptr_target() :
        cb_ptr_(new control_block<T>())
    {
        cb_ptr_->target = static_cast<T*>(this);
    }

    virtual ~automatic_ptr_target()
    {
        cb_ptr_->target = nullptr;
    }

    automatic_ptr_target(const automatic_ptr_target&) :
        cb_ptr_(new control_block<T>())
    {
        cb_ptr_->target = static_cast<T*>(this);
    }

    automatic_ptr_target(automatic_ptr_target&& rhs) :
        cb_ptr_(new control_block<T>())
    {
        cb_ptr_->target = static_cast<T*>(this);
        rhs.cb_ptr_->target = nullptr;
    }

    automatic_ptr_target& operator=(const automatic_ptr_target&)
    {}

    automatic_ptr_target& operator=(automatic_ptr_target&& rhs)
    {
        rhs.cb_ptr_->target = nullptr;
        automatic_ptr_target();
    }

private:
    std::shared_ptr<control_block<T>> cb_ptr_;
};

/** @brief A smart pointer type that is reset automatically on target object
 * destruction.
 *
 */
template<typename T>
class automatic_ptr
{
public:
    automatic_ptr() noexcept = default;
    automatic_ptr(const automatic_ptr& rhs) noexcept {
        rhs.sanitize();
        cb_ptr_ = rhs.cb_ptr_;
    }

    explicit automatic_ptr(const automatic_ptr_target<T>* target) noexcept :
        cb_ptr_(target->cb_ptr_)
    {}

    automatic_ptr& operator=(const automatic_ptr& rhs) noexcept {
        rhs.sanitize();
        cb_ptr_ = rhs.cb_ptr_;
    }

    automatic_ptr& operator=(const automatic_ptr_target<T>* target) noexcept {
        cb_ptr_ = target->cb_ptr_;
    }

    operator bool() const noexcept {
        sanitize();
        return cb_ptr_ && cb_ptr_->target != nullptr;
    }

    T* get() noexcept {
        sanitize();
        return cb_ptr_->target;
    }

    const T* get() const noexcept {
        sanitize();
        return cb_ptr_->target;
    }

    T* operator->() noexcept {
        sanitize();
        return cb_ptr_->target;
    }

    const T* operator->() const noexcept {
        sanitize();
        return cb_ptr_->target;
    }

    T& operator*() noexcept {
        sanitize();
        return *cb_ptr_->target;
    }

    const T& operator*() const noexcept {
        sanitize();
        return *cb_ptr_->target;
    }

    void reset() {
        cb_ptr_.reset();
    }

private:
    void sanitize() const noexcept {
        if (cb_ptr_ && cb_ptr_->target == nullptr)
            cb_ptr_.reset();
    }

    mutable std::shared_ptr<control_block<T>> cb_ptr_;
};

} // namespace decof

#endif // DECOF_AUTOMATIC_PTR_H
