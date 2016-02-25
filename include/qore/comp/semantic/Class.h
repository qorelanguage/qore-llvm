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
/// \brief Representation of classes during semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_CLASS_H_
#define INCLUDE_QORE_COMP_SEMANTIC_CLASS_H_

#include "qore/comp/semantic/Symbol.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a class.
 */
class Class : public SymbolBase<Symbol::Kind::Class> {

public:
    /**
     * \brief Returns the location of the class declaration.
     * \return the location of the class declaration.
     */
    const SourceLocation &getLocation() const {
        return location;
    }

    /**
     * \brief Returns the name of the class.
     * \return the name of the class
     */
    const std::string getName() const {
        return name;
    }

    /**
     * \brief Returns the full name of the class.
     * \return the full name of the class
     */
    std::string getFullName() const {
        return (parent ? (parent->isRoot() ? "" : parent->getFullName()) + "::" : "") + name;
    }

private:
    Class(Context &context, Namespace *parent, SourceLocation location, std::string name) : context(context),
            parent(parent), location(location), name(std::move(name)) {
    }

private:
    Context &context;
    Namespace *parent;
    SourceLocation location;
    std::string name;

    FRIEND_MAKE_UNIQUE;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_CLASS_H_
