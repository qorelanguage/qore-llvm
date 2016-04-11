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

    friend void incRef(qvalue) noexcept;
    friend void decRef(qvalue);
};

void incRef(qvalue p) noexcept {
    if (p.p) {
        LOG(p.p << " refInc: " << p.p->refCount << "->" << (p.p->refCount + 1));
        //XXX atomic
        ++p.p->refCount;
    }
}

void decRef(qvalue p) {
    if (p.p) {
        LOG(p.p << " refDec: " << p.p->refCount << "->" << (p.p->refCount - 1));
        if (!--p.p->refCount) {
            delete p.p;
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

    friend qvalue createString(const char *, qsize);
    friend qvalue convertIntToString(qvalue i);
    friend qvalue convertStringToInt(qvalue p);
    friend qvalue opAddStringString(qvalue left, qvalue right);
};

qvalue createString(const char *data, qsize length) {
    qvalue v;
    v.p = new QString(std::string(data, length));
    return v;
}

qvalue convertIntToString(qvalue i) {
    LOG("convertIntToString(" << i.i << ")");
    qvalue v;
    v.p = new QString(std::to_string(i.i));
    return v;
}

qvalue convertStringToInt(qvalue p) {
    assert(p.p->getType() == Type::String);
    LOG("convertStringToInt(" << p.p << ")");
    qvalue v;
    std::stringstream str(static_cast<QString *>(p.p)->str);
    str >> v.i;
    return v;
}

qvalue opAddIntInt(qvalue left, qvalue right) {
    qvalue v;
    v.i = left.i + right.i;
    return v;
}

qvalue opAddStringString(qvalue left, qvalue right) {
    assert(left.p->getType() == Type::String && right.p->getType() == Type::String);
    LOG("opAddStringString(" << left.p << ", " << right.p << ")");
    qvalue v;
    v.p = new QString(static_cast<QString *>(left.p)->str + static_cast<QString *>(right.p)->str);
    return v;
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

    friend qvalue int_box(qvalue i);
    friend qvalue int_unbox(qvalue p) noexcept;
};

qvalue int_box(qvalue i) {
    //XXX instances can be cached if QInt is immutable
    qvalue v;
    v.p = new QInt(i.i);
    return v;
}

qvalue int_unbox(qvalue p) noexcept {
    assert(p.p->getType() == Type::Int);
    qvalue v;
    v.i = static_cast<QInt *>(p.p)->i;
    return v;
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

void gv_read_lock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " read lock");
}

void gv_read_unlock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " read unlock");
}

void gv_write_lock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " write lock");
}

void gv_write_unlock(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " write unlock");
}

qvalue gv_get(GlobalVariable *gv) noexcept {
    LOG("GlobalVariable " << gv << " get");
    return gv->value;
}

void gv_set(GlobalVariable *gv, qvalue v) noexcept {
    LOG("GlobalVariable " << gv << " set");
    gv->value = v;
}

extern "C" qvalue qint_to_qvalue(qint i) noexcept {
    qvalue v;
    v.i = i;
    return v;
}

extern "C" void combine(Exception &original, Exception &secondary) {
//FIXME
}

static qvalue convert_any(qvalue src, Type type) {
    const meta::ConversionDesc &desc = meta::findConversion(src.p->getType(), type);
    if (src.p->getType() == Type::Int) {
        src = int_unbox(src);
    }
    return desc.f(src);
}

qvalue op_generic(Op o, qvalue left, qvalue right) {
    const meta::BinaryOperatorDesc &desc = meta::findOperator(o, left.p->getType(), right.p->getType());

    //FIXME exceptions are not handled correctly
    qvalue ret = desc.f(convert_any(left, desc.leftType), convert_any(right, desc.rightType));
    if (desc.retType == Type::Int) {
        ret = int_box(ret);
    }
    return ret;
}

qvalue op_add_any_any(qvalue left, qvalue right) {
    //left or right can be nullptr
    LOG("op_add_any_any: " << left.p << " + " << right.p);
    return op_generic(Op::Plus, left, right);
}

qvalue op_addeq_any_any(qvalue left, qvalue right) {
    //left or right can be nullptr
    LOG("op_addeq_any_any: " << left.p << " + " << right.p);
    return op_generic(Op::PlusEq, left, right);
}

} // namespace rt
} // namespace qore
