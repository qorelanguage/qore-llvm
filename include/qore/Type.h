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
#ifndef INCLUDE_QORE_TYPE_H_
#define INCLUDE_QORE_TYPE_H_

#include <memory>
#include <string>
#include "qore/common/Exceptions.h"

namespace qore {

class Type {

public:
    enum class Kind {
        Error,
        Any,
        Nothing,
        Bool,
        SoftBool,
        Int,
        IntOpt,
        SoftInt,
        String,
        StringOpt,
        SoftString,
        Object,
        ObjectOpt,
    };

public:
    using Ptr = std::unique_ptr<Type>;

public:
    static Ptr create(Kind kind, std::string name) {
        return Ptr(new Type(kind, name));
    }

    bool operator==(const Type &other) const {
        return this == &other;
    }

    bool operator!=(const Type &other) const {
        return !(*this == other);
    }

    Kind getKind() const {
        return kind;
    }

    const std::string &getName() const {
        return name;
    }

    static const Type &from(Kind kind) {
        switch (kind) {
            case Kind::Error:
                return Error;
            case Kind::Any:
                return Any;
            case Kind::Nothing:
                return Nothing;
            case Kind::Bool:
                return Bool;
            case Kind::SoftBool:
                return SoftBool;
            case Kind::Int:
                return Int;
            case Kind::IntOpt:
                return IntOpt;
            case Kind::String:
                return String;
            case Kind::StringOpt:
                return StringOpt;
            default:
                QORE_UNREACHABLE("Invalid Type::Kind: " << static_cast<int>(kind));
        }
    }

    bool isRefCounted() const {
        switch (kind) {
            case Kind::Nothing:
            case Kind::Bool:
            case Kind::SoftBool:
            case Kind::Int:
            case Kind::IntOpt:
            case Kind::SoftInt:
                return false;
            default:
                return true;
        }
    }

private:
    Type(Kind kind, std::string name) : kind(kind), name(std::move(name)) {
    }

private:
    Type(const Type &) = delete;
    Type(Type &&) = delete;
    Type &operator=(const Type &) = delete;
    Type &operator=(Type &&) = delete;

public:
    static const Type Error;
    static const Type Any;
    static const Type Nothing;
    static const Type Bool;
    static const Type SoftBool;
    static const Type Int;
    static const Type IntOpt;
    static const Type SoftInt;
    static const Type String;
    static const Type StringOpt;
    static const Type SoftString;

private:
    Kind kind;
    std::string name;
};

inline std::ostream &operator<<(std::ostream &os, const Type &type) {
    return os << type.getName();
}

} // namespace qore

#endif // INCLUDE_QORE_TYPE_H_
