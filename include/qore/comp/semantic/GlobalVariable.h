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
/// \brief Representation of global variables during semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_GLOBALVARIABLE_H_
#define INCLUDE_QORE_COMP_SEMANTIC_GLOBALVARIABLE_H_

#include <string>
#include "qore/comp/semantic/Symbol.h"
#include "qore/comp/semantic/Type.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a global variable.
 */
class GlobalVariable : public Symbol {

public:
    /**
     * \brief Creates a new instance.
     * \param ctx the compiler context
     * \param parent the parent scope
     * \param name the name of the variable
     * \param location the location of the declaration
     * \param type the type of the variable
     */
    GlobalVariable(Context &ctx, NamedScope &parent, String::Ref name, SourceLocation location, Type::Ref type)
         : ctx(ctx), parent(parent), name(name), location(location), type(type) {
    }

    Kind getKind() const override {
        return Kind::GlobalVariable;
    }

    String::Ref getName() const override {
        return name;
    }

    SourceLocation getLocation() const override {
        return location;
    }

    /**
     * \brief Returns the full name of the global variable.
     * \return the full name of the global variable
     */
    std::string getFullName() const {
        return parent.getFullName() + "::" + ctx.getStringTable().get(name);
    }

    /**
     * \brief Returns the type of the global variable.
     * \return the type of the global variable
     */
    const Type::Ref &getType() const {
        return type;
    }

private:
    Context &ctx;
    NamedScope &parent;
    String::Ref name;
    SourceLocation location;
    Type::Ref type;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_GLOBALVARIABLE_H_
