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
/// \brief Qore runtime functions.
///
//------------------------------------------------------------------------------
#include "qore/rt/Func.h"
#include <cassert>
#include <iostream>
#include <string>
#include "qore/common/Exceptions.h"
#include "qore/common/Logging.h"
#include "qore/common/Loggable.h"
#include "qore/rt/Meta.h"

namespace qore {
namespace rt {

///////////////////////////////////////////////////////////////
/// Any
///////////////////////////////////////////////////////////////
class QAny : public log::Loggable {

public:
    QAny() : refCount(1) {
    }

    virtual Type getType() const = 0;

protected:
    virtual ~QAny() = default;

private:
    QAny(const QAny &) = delete;
    QAny(QAny &&) = delete;
    QAny &operator=(const QAny &) = delete;
    QAny &operator=(QAny &&) = delete;

private:
    qsize refCount;

    friend void incRef(qptr) noexcept;
    friend void decRef(qptr);
};

void incRef(qptr p) noexcept {
    if (p) {
        LOG(p << " refInc: " << p->refCount << "->" << (p->refCount + 1));
        //XXX atomic
        ++p->refCount;
    }
}

void decRef(qptr p) {
    if (p) {
        LOG(p << " refDec: " << p->refCount << "->" << (p->refCount - 1));
        if (!--p->refCount) {
            delete p;
        }
    }
}

///////////////////////////////////////////////////////////////
/// String
///////////////////////////////////////////////////////////////
class QString : public QAny {

public:
    Type getType() const override {
        return Type::String;
    }

private:
    explicit QString(std::string str) : str(std::move(str)) {
        LOG(this << " created");
    }

protected:
    ~QString() {
        LOG(this << " destroyed");
    }

protected:
    WRITE_TO_LOG("QString \"" << str << "\"")

private:
    std::string str;

    friend qptr createString(const char *, qsize);
    friend qptr convertIntToString(qint i);
    friend qint convertStringToInt(qptr p);
    friend qptr opAddStringString(qptr l, qptr r);
};

qptr createString(const char *data, qsize length) {
    return new QString(std::string(data, length));
}

qptr convertIntToString(qint i) {
    LOG("convertIntToString(" << i << ")");
    return new QString(std::to_string(i));
}

qint convertStringToInt(qptr p) {
    assert(p->getType() == Type::String);
    LOG("convertStringToInt(" << p << ")");
    qint i;
    std::stringstream str(static_cast<QString *>(p)->str);
    str >> i;
    return i;
}

qptr opAddStringString(qptr l, qptr r) {
    assert(l->getType() == Type::String && r->getType() == Type::String);
    LOG("opAddStringString(" << l << ", " << r << ")");
    return new QString(static_cast<QString *>(l)->str + static_cast<QString *>(r)->str);
}



///////////////////////////////////////////////////////////////
/// Int
///////////////////////////////////////////////////////////////
class QInt : public QAny {

public:
    Type getType() const override {
        return Type::Int;
    }

private:
    explicit QInt(qint i) : i(i) {
        LOG(this << " created");
    }

protected:
    ~QInt() {
        LOG(this << " destroyed");
    }

protected:
    WRITE_TO_LOG("QInt \"" << i << "\"")

private:
    qint i;

    friend qptr int_box(qint i);
    friend qint int_unbox(qptr p) noexcept;
};

qptr int_box(qint i) {
    //XXX instances can be cached if QInt is immutable
    return new QInt(i);
}

qint int_unbox(qptr p) noexcept {
    assert(p->getType() == Type::Int);
    return static_cast<QInt *>(p)->i;
}



///////////////////////////////////////////////////////////////
/// GlobalVariable
///////////////////////////////////////////////////////////////
class RwLock {
public:
    void readLock() {
        LOG("READ LOCK");
    }
    void writeLock() {
        LOG("WRITE LOCK");
    }
    void unlock() {
        LOG("UNLOCK");
    }
};

struct GlobalVariable {
    RwLock lock;
    qvalue value;
};

GlobalVariable *gv_create(qvalue init) {
    GlobalVariable *gv = new GlobalVariable();
    LOG("GlobalVariable " << gv << " created");
    gv->value = init;
    return gv;
}

qvalue gv_free(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " destroyed");
    qvalue v =  gv->value;
    delete gv;
    return v;
}

qvalue gv_read_lock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " read lock");
    return gv->value;
}

void gv_read_unlock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " read unlock");
}

qvalue *gv_write_lock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " write lock");
    return &gv->value;
}

void gv_write_unlock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " write unlock");
}

extern "C" qint qvalue_to_qint(qvalue v) noexcept {
    return v.i;
}

extern "C" qint *qvalue_ptr_to_qint_ptr(qvalue *v) noexcept {
    return &v->i;
}

extern "C" qptr qvalue_to_qptr(qvalue v) noexcept {
    return v.p;
}

extern "C" qptr *qvalue_ptr_to_qptr_ptr(qvalue *v) noexcept {
    return &v->p;
}

extern "C" qvalue qint_to_qvalue(qint i) noexcept {
    qvalue v;
    v.i = i;
    return v;
}

extern "C" qvalue qptr_to_qvalue(qptr p) noexcept {
    qvalue v;
    v.p = p;
    return v;
}

extern "C" void combine(Exception &original, Exception &secondary) {
//FIXME
}

static qptr convert_any(qptr src, Type type) {
    Conversion c = meta::findConversion(src->getType(), type);
    switch (c) {
        case Conversion::Identity:
            return src;
        case Conversion::IntToString:
            return convertIntToString(int_unbox(src));
        default:
            QORE_NOT_IMPLEMENTED("throw an exception");
    }
}

static qint convert_any_to_softint(qptr src) {
    Conversion c = meta::findConversion(src->getType(), Type::SoftInt);
    switch (c) {
        case Conversion::Identity:
            return int_unbox(src);
        case Conversion::StringToInt:
            return convertStringToInt(src);
        //float to int, etc.
        default:
            QORE_NOT_IMPLEMENTED("throw an exception");
    }
}

qptr op_generic(Op o, qptr left, qptr right) {
    Operator op = meta::findOperator(o, left->getType(), right->getType());
    switch (op) {
        case Operator::IntPlusInt:
            return int_box(convert_any_to_softint(left) + convert_any_to_softint(right));
        case Operator::StringPlusString:
            //FIXME exception are not handled correctly
            return opAddStringString(convert_any(left, Type::String), convert_any(right, Type::String));
        default:
            QORE_NOT_IMPLEMENTED("throw an exception");
    }
}

qptr op_add_any_any(qptr left, qptr right) {
    //left or right can be nullptr
    LOG("op_add_any_any: " << left << " + " << right);
    return op_generic(Op::Plus, left, right);
}

qptr op_addeq_any_any(qptr left, qptr right) {
    //left or right can be nullptr
    LOG("op_addeq_any_any: " << left << " + " << right);
    return op_generic(Op::PlusEq, left, right);
}

} // namespace rt
} // namespace qore
