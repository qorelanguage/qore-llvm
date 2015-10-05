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
/// \brief TODO File description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_RUNTIME_QORESTRING_H_
#define INCLUDE_QORE_RUNTIME_QORESTRING_H_

#include "qore/common/Logging.h"
#include <algorithm>
#include <cassert>

namespace qore {
namespace rt {

class QoreReferenceCounted {

public:
    using RefCountType = int;

protected:
    QoreReferenceCounted() : refCount(1) {
    }

public:
    virtual ~QoreReferenceCounted() {
    }

    void deref() {
        if (!--refCount) {
            delete this;
        }
    }

    void ref() {
        ++refCount;
    }

    RefCountType getRefCount() const {
        return refCount;
    }

private:
    QoreReferenceCounted(const QoreReferenceCounted &) = delete;
    QoreReferenceCounted(QoreReferenceCounted &&) = delete;
    QoreReferenceCounted &operator=(const QoreReferenceCounted &) = delete;
    QoreReferenceCounted &operator=(QoreReferenceCounted &&) = delete;

private:
    RefCountType refCount;
};

class QoreString : public QoreReferenceCounted {

public:
    QoreString(const char *str) : value(str) {
        CLOG("RT", "Creating " << this);
    }

    QoreString(std::string str) : value(std::move(str)) {
        CLOG("RT", "Creating " << this);
    }

    ~QoreString() {
        CLOG("RT", "Destroying " << this);
    }

    const std::string &getValue() const {  //encoding, thread safety
        return value;
    }

    void trim() {
        assert(getRefCount() <= 2);         // 1 ... working on a fresh clone, 2 ... the lvalue and 'this' are the only referees
        if (value.find('a') != std::string::npos) {
            throw 123;
        }
        auto wsfront = std::find_if_not(value.begin(), value.end(), [](int c){return std::isspace(c);});
        auto wsback = std::find_if_not(value.rbegin(), value.rend(), [](int c){return std::isspace(c);}).base();
        value = (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
    }

private:
    std::string value;      //TODO this will include encoding info

private:
    friend std::ostream &operator<<(std::ostream &os, const QoreString &qs) {
        return os << "QoreString[" << qs.getRefCount() << "]{" << static_cast<const void*>(&qs) << "}:\"" << qs.value << "\"";
    }

    friend std::ostream &operator<<(std::ostream &os, const QoreString *qs) {
        return os << *qs;
    }
};


} // namespace rt
} // namespace qore

#endif // INCLUDE_QORE_RUNTIME_QORESTRING_H_
