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
/// \brief Defines the base class for all Qore values that are reference-counted.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_ANY_H_
#define INCLUDE_QORE_CORE_ANY_H_

#include "qore/core/RefCounted.h"
#include "qore/core/Type.h"
#include "qore/core/Value.h"

namespace qore {

/**
 * \brief Base class for all Qore values that are reference-counted.
 */
class Any : public RefCounted {

public:
    /**
     * \brief Returns the type of the value represented by this instance.
     * \return the type of the value represented by this instance
     */
    virtual const Type &getType() const = 0;

protected:
    Any() = default;

private:
    Any(const Any &) = delete;
    Any(Any &&) = delete;
    Any &operator=(const Any &) = delete;
    Any &operator=(Any &&) = delete;
};

/**
 * \brief Simple smart pointer for qvalue. The decrease of reference count in destructor is done optionally based
 * on a `refCounted` parameter specified during construction.
 */
template<>
class auto_ptr<qvalue> {

public:
    auto_ptr() : refCounted(false) {
        value.p = nullptr;
    }

    /**
     * \brief Creates the pointer.
     *
     * Takes over the responsibility of optionally decreasing the reference count of a qvalue.
     * \param value the qvalue (with increased reference count if `refCounted` is true)
     * \param refCounted true if value needs its reference count decreased
     */
    auto_ptr(qvalue value, bool refCounted) : value(value), refCounted(refCounted) {
    }

    /**
     * \brief Optionally decreases the reference count of the value.
     */
    ~auto_ptr() {
        if (refCounted && value.p) {
            value.p->decRefCount();
        }
    }

    /**
     * \brief Move constructor.
     *
     * Takes over the responsibility of optionally decreasing the reference count of the value.
     * \param src the source pointer, will be left empty after this call
     */
    auto_ptr(auto_ptr<qvalue> &&src) : value(src.value), refCounted(src.refCounted) {
        src.value.p = nullptr;
        src.refCounted = false;
    }

    /**
     * \brief Move assignment.
     *
     * Takes over the responsibility of optionally decreasing the reference count of the value.
     * \param src the source pointer, will be left empty after this call
     * \return this
     */
    auto_ptr &operator=(auto_ptr<qvalue> &&src) {
        value = src.value;
        refCounted = src.refCounted;
        src.value.p = nullptr;
        src.refCounted = false;
        return *this;
    }

    /**
     * \brief Dereferences the pointer.
     * \return the qvalue pointed to by this pointer
     */
    qvalue &operator*() {
        return value;
    }

    /**
     * \brief Hands over the responsibility of optionally decreasing the reference count of the value and
     * leaves this instance empty.
     * \return the value
     */
    qvalue release() {
        qvalue r = value;
        value.p = nullptr;
        refCounted = false;
        return r;
    }

private:
    auto_ptr(const auto_ptr &) = delete;
    auto_ptr &operator=(const auto_ptr &) = delete;

private:
    qvalue value;
    bool refCounted;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_ANY_H_
