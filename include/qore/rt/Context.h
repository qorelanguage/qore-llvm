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
#ifndef INCLUDE_QORE_RT_CONTEXT_H_
#define INCLUDE_QORE_RT_CONTEXT_H_

#include <cassert>
#include <vector>
#include "qore/Id.h"
#include "qore/rt/Types.h"
#include "qore/rt/Func.h"
#include "qore/common/Exceptions.h"

namespace qore {
namespace rt {

class StringHolder {

public:
    StringHolder() {
        str.p = nullptr;
    }

    ~StringHolder() {
        decRef(str);
    }

    StringHolder(StringHolder &&src) : str(src.str) {
        src.str.p = nullptr;
    }

    StringHolder &operator=(StringHolder &&src) {
        str = src.str;
        src.str.p = nullptr;
        return *this;
    }


    void set(qptr p) {
        assert(!str.p);
        str.p = p;
    }

    qvalue get() {
        assert(str.p != nullptr);
        return str;
    }

private:
    StringHolder(const StringHolder &) = delete;
    StringHolder &operator=(const StringHolder &) = delete;

private:
    qvalue str;
};

///////////////////////////////////////////////////////////////
/// GlobalVariable
///////////////////////////////////////////////////////////////
class RwLock {
};

struct GlobalVariable {

public:
    GlobalVariable() : refCounted(false) {
        value.p = nullptr;
    }

    ~GlobalVariable() {
        if (refCounted) {
            decRef(value);
        }
        LOG("GlobalVariable " << this << " destroyed");
    }

    void init(bool refCounted, qvalue value) {
        LOG("GlobalVariable " << this << " created");
        this->refCounted = refCounted;
        this->value = value;
    }

    RwLock lock;
    bool refCounted;
    qvalue value;
};

class Context {

public:
    void createString(Id id, const char *value, qsize length);

    qvalue loadString(Id id) {
        assert(id < strings.size());
        return strings[id].get();
    }

    void createGlobal(Id id, bool refCounted, qvalue initValue) {
        globals.resize(id + 1);
        globals[id].init(refCounted, initValue);
    }

    void gv_read_lock(Id id) noexcept {
        LOG("GlobalVariable " << id << " read lock");
    }

    void gv_read_unlock(Id id) noexcept {
        LOG("GlobalVariable " << id << " read unlock");
    }

    void gv_write_lock(Id id) noexcept {
        LOG("GlobalVariable " << id << " write lock");
    }

    void gv_write_unlock(Id id) noexcept {
        LOG("GlobalVariable " << id << " write unlock");
    }

    qvalue gv_get(Id id) noexcept {
        assert(id < globals.size());
        LOG("GlobalVariable " << id << " get");
        return globals[id].value;
    }

    void gv_set(Id id, qvalue v) noexcept {
        assert(id < globals.size());
        LOG("GlobalVariable " << id << " set");
        globals[id].value = v;
    }

private:
    std::vector<StringHolder> strings;
    std::vector<GlobalVariable> globals;
};

} // namespace rt
} // namespace qore

#endif // INCLUDE_QORE_RT_CONTEXT_H_
