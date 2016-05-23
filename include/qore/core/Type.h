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
/// \brief Definition of runtime Qore types.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_TYPE_H_
#define INCLUDE_QORE_CORE_TYPE_H_

#include <memory>
#include <string>
#include "qore/core/util/Debug.h"

namespace qore {

/**
 * \brief Describes a type in the Qore runtime.
 *
 * It is assumed that for each Qore type at most one instance of this class exists (comparison operators use
 * addresses).
 */
class Type {

private:
    /**
     * \brief Enumeration of all kinds of types.
     */
    enum class Kind {
        Error,          //!< Special type used during semantic analysis for error recovery.
        FunctionGroup,  //!< Special type used during semantic analysis for FunctionGroupExpression.
        Any,            //!< The `any` type.
        Nothing,        //!< The 'nothing' type used by functions with no return value and the NOTHING literal.
        Bool,           //!< The 'bool' type.
        SoftBool,       //!< The 'softbool' type.
        Int,            //!< The 'int' type.
        IntOpt,         //!< The '*int' type.
        SoftInt,        //!< The 'softint' type.
        String,         //!< The 'string' type.
        StringOpt,      //!< The '*string' type.
        SoftString,     //!< The 'softstring' type.
        Object,         //!< The 'object' type.
        ObjectOpt,      //!< The '*object' type.
    };

public:
    using Ptr = std::unique_ptr<Type>;      //!< Pointer type.

public:
    /**
     * \brief Allocates a new type for a class.
     * \param className the full name of the class.
     * \param optional true if the type accepts `NOTHING`.
     * \return a pointer to the allocated type
     */
    static Ptr createForClass(std::string className, bool optional) {
        return Ptr(optional ? new Type(Kind::ObjectOpt, "*" + className)
                : new Type(Kind::Object, std::move(className)));
    }

    /**
     * \brief Compares two types for equality.
     * \param other the type to compare `this` to
     * \return true if the type represented by this instance is the same as the type represented by `other`
     */
    bool operator==(const Type &other) const {
        return this == &other;
    }

    /**
     * \brief Compares two types for inequality.
     * \param other the type to compare `this` to
     * \return true if the type represented by this instance is not the same as the type represented by `other`
     */
    bool operator!=(const Type &other) const {
        return !(*this == other);
    }

    /**
     * \brief Returns the name of the type.
     * \return the name of the type
     */
    const std::string &getName() const {
        return name;
    }

    /**
     * \brief Returns true if the values of this type use reference counting.
     * \return true if the values of this type use reference counting
     */
    bool isRefCounted() const {
        switch (kind) {
            case Kind::Nothing:
            case Kind::Bool:
            case Kind::SoftBool:
            case Kind::Int:
            case Kind::SoftInt:
                return false;
            default:
                return true;
        }
    }

    /**
     * \brief Returns true if `NOTHING` is a valid value for this type.
     * \return true if `NOTHING` is a valid value for this type
     */
    bool acceptsNothing() const {
        return *this == Type::Error || *this == Type::Any || *this == Type::Nothing
                || *this == Type::IntOpt || *this == Type::StringOpt || kind == Type::Kind::ObjectOpt;
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
    static const Type Error;        //!< Special type used during semantic analysis for error recovery.
    static const Type FunctionGroup;//!< Special type used during semantic analysis for FunctionGroupExpression.
    static const Type Any;          //!< The `any` type.
    static const Type Nothing;      //!< The type used by functions with no return value and the NOTHING literal.
    static const Type Bool;         //!< The 'bool' type.
    static const Type SoftBool;     //!< The 'softbool' type.
    static const Type Int;          //!< The 'int' type.
    static const Type IntOpt;       //!< The '*int' type.
    static const Type SoftInt;      //!< The 'softint' type.
    static const Type String;       //!< The 'string' type.
    static const Type StringOpt;    //!< The '*string' type.
    static const Type SoftString;   //!< The 'softstring' type.

private:
    Kind kind;
    std::string name;
};

/**
 * \brief Writes the name of the type to an output stream.
 * \param os the output stream
 * \param type the type
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const Type &type) {
    return os << type.getName();
}

} // namespace qore

#endif // INCLUDE_QORE_CORE_TYPE_H_
