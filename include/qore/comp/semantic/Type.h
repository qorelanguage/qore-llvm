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

private:
    explicit TypeReference(std::shared_ptr<Type> type) : type(type) {
        assert(type != nullptr);
    }

private:
    std::shared_ptr<Type> type;

    friend class TypeRegistry;
    friend std::ostream &operator<<(std::ostream &os, const TypeReference &r) {
        return os << r.type->getName();
    }
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_TYPE_H_
