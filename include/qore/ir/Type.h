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
#ifndef INCLUDE_QORE_IR_TYPE_H_
#define INCLUDE_QORE_IR_TYPE_H_

#include <memory>
#include <string>
#include "qore/rt/Types.h"

namespace qore {
namespace ir {

class Type {

public:
    enum class Kind {
        Builtin
    };

public:
    virtual ~Type() = default;

    virtual Kind getKind() const = 0;

    bool operator==(const Type &other) const {
        return this == &other;
    }

    bool operator!=(const Type &other) const {
        return !(*this == other);
    }

protected:
    explicit Type(rt::qvalue_type rtType) : rtType(rtType) {
    }

private:
    Type(const Type &) = delete;
    Type(Type &&) = delete;
    Type &operator=(const Type &) = delete;
    Type &operator=(Type &&) = delete;

public:
    rt::qvalue_type rtType; //FIXME public

};

class BuiltinType : public Type {

public:
    Kind getKind() const override {
        return Kind::Builtin;
    }

    rt::Type getRuntimeType() const {
        return runtimeType;
    }

private:
    BuiltinType(rt::Type runtimeType, rt::qvalue_type rtType) : Type(rtType), runtimeType(runtimeType) {
    }

private:
    rt::Type runtimeType;
    friend class Types;
};

class Types {

public:
    static BuiltinType Any;
    static BuiltinType Nothing;
    static BuiltinType Int;
    static BuiltinType SoftInt;
    static BuiltinType String;
    static BuiltinType SoftString;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_TYPE_H_