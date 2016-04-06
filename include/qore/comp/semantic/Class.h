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

#include <string>
#include "qore/comp/semantic/Scope.h"
#include "qore/comp/semantic/Symbol.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a class.
 */
class Class : public NamedScope, public Symbol {

public:
    /**
     * \brief Creates a new instance.
     * \param ctx the compiler context
     * \param parent the parent scope
     * \param name the name of the class
     * \param location the location of the declaration
     */
    Class(Context &ctx, NamedScope &parent, String::Ref name, SourceLocation location)
        : ctx(ctx), parent(parent), name(name), location(location) {
    }

    Kind getKind() const override {
        return Kind::Class;
    }

    String::Ref getName() const override {
        return name;
    }

    SourceLocation getLocation() const override {
        return location;
    }

    std::string getFullName() const override {
        return parent.getFullName() + "::" + ctx.getStringTable().get(name);
    }

    Class &resolveClass(const ast::Name &name) const override {
        return parent.resolveClass(name);
    }

private:
    Context &ctx;
    NamedScope &parent;
    String::Ref name;
    SourceLocation location;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_CLASS_H_
