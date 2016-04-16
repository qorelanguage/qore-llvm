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
#ifndef INCLUDE_QORE_COMP_AS_TYPE_H_
#define INCLUDE_QORE_COMP_AS_TYPE_H_

#include <string>
#include "qore/common/Exceptions.h"
#include "qore/rt/Types.h"

namespace qore {
namespace comp {
namespace as {

class Type {

public:
    Type(rt::Type runtimeType, std::string name, bool optional, bool primitive) : name(std::move(name)),
            optional(optional), primitive(primitive), runtimeType(runtimeType) {
    }

    bool operator==(const Type &other) const {
        return this == &other;
    }

    bool operator!=(const Type &other) const {
        return !(*this == other);
    }

    const std::string &getName() const {
        return name;
    }

    rt::Type getRuntimeType() const {
        return runtimeType;
    }

    bool isOptional() const {
        return optional;
    }

    bool isPrimitive() const {
        return primitive;
    }

    static const Type &from(rt::Type t) {
        switch (t) {
            case rt::Type::Error:
                return Error;
            case rt::Type::Int:
                return Int;
            case rt::Type::String:
                return String;
            case rt::Type::Any:
                return Any;
            case rt::Type::Nothing:
                return Nothing;
            default:
                QORE_NOT_IMPLEMENTED("");
        }
    }

public:
    static const Type Error;
    static const Type Int;
    static const Type IntOpt;
    static const Type String;
    static const Type StringOpt;
    static const Type Any;
    static const Type Nothing;

private:
    Type(const Type &) = delete;
    Type(Type &&) = delete;
    Type &operator=(const Type &) = delete;
    Type &operator=(Type &&) = delete;

private:
    std::string name;
    bool optional;
    bool primitive;
    rt::Type runtimeType;
};

} // namespace as
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AS_TYPE_H_
