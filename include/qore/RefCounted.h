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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_REFCOUNTED_H_
#define INCLUDE_QORE_REFCOUNTED_H_

#include "qore/common/Exceptions.h"
#include "qore/common/Logging.h"
#include "qore/common/Loggable.h"
#include "qore/Exception.h"

namespace qore {

class RefCounted : public log::Loggable {

private:
    using RefCount = size_t;

public:
    RefCounted() : refCount(1) {
    }

    void incRefCount() noexcept {
        LOG(this << " incRefCount: " << refCount << "->" << (refCount + 1));
        //XXX atomic
        ++refCount;
    }

    void decRefCount() noexcept(false) {
        LOG(this << " decRefCount: " << refCount << "->" << (refCount - 1));
        //XXX atomic
        if (!--refCount) {
            delete this;
        }
    }

protected:
    virtual ~RefCounted() noexcept(false) {
    }

private:
    RefCounted(const RefCounted &) = delete;
    RefCounted(RefCounted &&) = delete;
    RefCounted &operator=(const RefCounted &) = delete;
    RefCounted &operator=(RefCounted &&) = delete;

private:
    RefCount refCount;
};

template<typename T>
class auto_ptr {

public:
    explicit auto_ptr(T *ptr = nullptr) : ptr(ptr) {
    }

    ~auto_ptr() {
        if (ptr) {
            try {
                ptr->decRefCount();
            } catch (Exception &e) {
                QORE_NOT_IMPLEMENTED("");
            }
        }
    }

    auto_ptr(auto_ptr &&src) : ptr(src.ptr) {
        src.ptr = nullptr;
    }

    auto_ptr &operator=(auto_ptr &&src) {
        ptr = src.ptr;
        src.ptr = nullptr;
        return *this;
    }

    T &operator*() {
        return *get();
    }

    T *operator->() {
        return get();
    }

    T *get() const {
        return ptr;
    }

    operator bool() const {
        return ptr != nullptr;
    }

    bool operator!() const {
        return ptr == nullptr;
    }

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

#endif // INCLUDE_QORE_REFCOUNTED_H_
