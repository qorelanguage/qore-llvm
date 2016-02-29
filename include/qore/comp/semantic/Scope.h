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
/// \brief Interface for scopes.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_SCOPE_H_
#define INCLUDE_QORE_COMP_SEMANTIC_SCOPE_H_

#include <string>

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Base class for all named objects.
 */
class NamedObject {

public:
    /**
     * \brief Identifies the type of the named object.
     */
    enum class Kind {
        Constant,                   //!< Identifies instances of \ref Constant.
        FunctionGroup,              //!< Identifies instances of \ref FunctionGroup.
        GlobalVariable,             //!< Identifies instances of \ref GlobalVariable.
    };

public:
    using Ptr = std::unique_ptr<NamedObject>;               //!< Pointer type.

public:
    virtual ~NamedObject() = default;

    /**
     * \brief Returns the type of the symbol.
     * \return the type of the symbol
     */
    virtual Kind getKind() const = 0;

    /**
     * \brief Returns the location of the declaration.
     * \return the location of the declaration.
     */
    virtual const SourceLocation &getLocation() const = 0;

protected:
    NamedObject() = default;

private:
    NamedObject(const NamedObject &) = delete;
    NamedObject(NamedObject &&) = delete;
    NamedObject &operator=(const NamedObject &) = delete;
    NamedObject &operator=(NamedObject &&) = delete;
};

/**
 * \brief Helper template for extending \ref NamedObject.
 */
template<NamedObject::Kind kind> class NamedObjectBase : public NamedObject {

public:
    /**
     * \brief Identifies the named object type.
     */
    static constexpr Kind NamedObjectKind = kind;

    /**
     * \brief Returns the type of the named object.
     * \return the type of the named object
     */
    Kind getKind() const override {
        return kind;
    }
};

/**
 * \brief Interface for symbol name resolution.
 */
class Scope {

public:
    virtual ~Scope() = default;

protected:
    Scope() = default;

private:
    Scope(const Scope &) = delete;
    Scope(Scope &&) = delete;
    Scope &operator=(const Scope &) = delete;
    Scope &operator=(Scope &&) = delete;
};

/**
 * \brief Interface for named scopes (namespaces and classes).
 */
class NamedScope : public Scope {

public:
    /**
     * \brief Returns the full name of the named scope.
     * \return the full name of the named scope
     */
    virtual std::string getFullName() const = 0;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_SCOPE_H_
