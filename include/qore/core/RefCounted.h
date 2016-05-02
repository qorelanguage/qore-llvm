//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 Qore Technologies
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
///
/// \file
/// \brief Defines the base class for reference-counted objects and related utility classes.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_REFCOUNTED_H_
#define INCLUDE_QORE_CORE_REFCOUNTED_H_

#include <atomic>
#include "qore/core/Defs.h"
#include "qore/core/util/Debug.h"
#include "qore/core/util/Loggable.h"

namespace qore {

/**
 * \brief Base class for reference-counted objects.
 *
 * Instances must be allocated on the heap and the delete operator must not be used on them explicitly.
 */
class RefCounted : public util::Loggable {

public:
    /**
     * \brief Increases the reference count.
     */
    void incRefCount() noexcept {
        LOG(this << " incRefCount: " << refCount << "->" << (refCount + 1));
        ++refCount;
    }

    /**
     * \brief Decreases the reference count and if it drops to zero, commits suicide.
     */
    void decRefCount() noexcept {
        LOG(this << " decRefCount: " << refCount << "->" << (refCount - 1));
        if (!--refCount) {
            delete this;
        }
    }

protected:
    /**
     * \brief Constructs the object with reference count set to one.
     */
    RefCounted() : refCount(1) {
    }

    /**
     * \brief Destructor that will be invoked when the reference count drops to zero.
     */
    virtual ~RefCounted() {
    }

private:
    RefCounted(const RefCounted &) = delete;
    RefCounted(RefCounted &&) = delete;
    RefCounted &operator=(const RefCounted &) = delete;
    RefCounted &operator=(RefCounted &&) = delete;

private:
    std::atomic<Size> refCount;
};

/**
 * \brief Simple smart pointer for reference-counted objects.
 */
template<typename T>
class auto_ptr {

public:
    /**
     * \brief Creates the pointer.
     *
     * Takes over the responsibility of decreasing the reference count of the object.
     * \param ptr the object with increased reference count.
     */
    explicit auto_ptr(T *ptr = nullptr) : ptr(ptr) {
    }

    /**
     * \brief Decreases the reference count of the object.
     */
    ~auto_ptr() {
        if (ptr) {
            ptr->decRefCount();
        }
    }

    /**
     * \brief Move constructor.
     *
     * Takes over the responsibility of decreasing the reference count of the object.
     * \param src the source pointer, will be left empty after this call
     */
    auto_ptr(auto_ptr &&src) : ptr(src.ptr) {
        src.ptr = nullptr;
    }

    /**
     * \brief Move assignment.
     *
     * Takes over the responsibility of decreasing the reference count of the object.
     * \param src the source pointer, will be left empty after this call
     * \return this
     */
    auto_ptr &operator=(auto_ptr &&src) {
        ptr = src.ptr;
        src.ptr = nullptr;
        return *this;
    }

    /**
     * \brief Dereferences the pointer.
     * \return the object pointed to by this instance
     */
    T &operator*() {
        assert(ptr);
        return *get();
    }

    /**
     * \brief Dereferences the pointer.
     * \return a pointer to the object pointed to by this instance
     */
    T *operator->() {
        assert(ptr);
        return get();
    }

    /**
     * \brief Returns a raw pointer to the object pointed to by this instance. Does not affect the referece count.
     * \return a pointer to the object pointed to by this instance
     */
    T *get() const {
        return ptr;
    }

    /**
     * \brief Returns true if this instance is not empty, i.e. points to some object.
     */
    operator bool() const {
        return ptr != nullptr;
    }

    /**
     * \brief Returns true if this instance is empty, i.e. does not point to any object.
     */
    bool operator!() const {
        return ptr == nullptr;
    }

    /**
     * \brief Duplicates the pointer by increasing the reference count of the object.
     * \return a new smart pointer to the same object
     */
    auto_ptr<T> dup() const {
        assert(ptr);
        ptr->incRefCount();
        return auto_ptr<T>(ptr);
    }

private:
    auto_ptr(const auto_ptr &) = delete;
    auto_ptr &operator=(const auto_ptr &) = delete;

private:
    T *ptr;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_REFCOUNTED_H_
