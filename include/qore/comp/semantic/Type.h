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
/// \brief Type representation during semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_TYPE_H_
#define INCLUDE_QORE_COMP_SEMANTIC_TYPE_H_

#include <string>

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Type representation during semantic analysis.
 */
class Type {

public:
    using Ref = class TypeReference;                        //!< Reference type.

public:
    virtual ~Type() = default;

    /**
     * \brief Returns the name of the type.
     * \return the name of the type
     */
    virtual std::string getName() const = 0;

protected:
    Type() = default;

private:
    Type(const Type &) = delete;
    Type(Type &&) = delete;
    Type &operator=(const Type &) = delete;
    Type &operator=(Type &&) = delete;
};

/**
 * \brief Reference to a type
 */
class TypeReference {

public:
    /**
     * \brief Constructs an invalid type reference.
     */
    TypeReference() : type(nullptr) {
    }

private:
    explicit TypeReference(const Type *type) : type(type) {
    }

private:
    const Type *type;

    friend class TypeRegistry;
    friend std::ostream &operator<<(std::ostream &os, const TypeReference &r) {
        if (!r.type) {
            return os << "<unknown>";
        }
        return os <<r.type->getName();
    }
};

/**
 * \brief Represents a built-in type.
 */
class BuiltinType : public Type {

public:
    /**
     * \brief Creates a type with given name.
     * \param name the name of the type
     */
    explicit BuiltinType(std::string name) : name(std::move(name)) {
    }

    std::string getName() const override {
        return name;
    }

private:
    std::string name;
};

/**
 * \brief Represents a type defined by a class.
 */
class ClassType : public Type {

public:
    using Ptr = std::unique_ptr<ClassType>;                 //!< Pointer type.

public:
    /**
     * \brief Creates the type for given class.
     * \param clazz the class
     * \return pointer to the new instance
     */
    static Ptr create(Class &clazz) {
        return Ptr(new ClassType(clazz));
    }

    std::string getName() const override;

private:
    explicit ClassType(Class &clazz) : clazz(clazz) {
    }

private:
    Class &clazz;
};

/**
 * \brief Represents the type *T for a type T.
 */
class AsteriskType : public Type {

public:
    using Ptr = std::unique_ptr<AsteriskType>;              //!< Pointer type.

public:
    /**
     * \brief Creates the type *T for given type T.
     * \param type the original type
     * \return pointer to the new instance
     */
    static Ptr create(const Type &type) {
        return Ptr(new AsteriskType(type));
    }

    std::string getName() const override {
        return  "*" + type.getName();
    }

private:
    explicit AsteriskType(const Type &type) : type(type) {
    }

private:
    const Type &type;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_TYPE_H_
