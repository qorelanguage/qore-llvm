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
#ifndef INCLUDE_QORE_COMP_SEM_SCOPE_H_
#define INCLUDE_QORE_COMP_SEM_SCOPE_H_

#include "qore/comp/as/Type.h"
#include "qore/comp/ast/Type.h"

namespace qore {
namespace comp {
namespace sem {

class LocalVariable;
class GlobalVariableInfo;

class Symbol {

public:
    enum class Kind {
        Global,
        Local,
    };

public:
    explicit Symbol(const LocalVariable &v) : kind(Kind::Local), ptr(&v) {
    }

    explicit Symbol(const GlobalVariableInfo &v) : kind(Kind::Global), ptr(&v) {
    }

    Kind getKind() const {
        return kind;
    }

    const LocalVariable &asLocal() const {
        assert(kind == Kind::Local);
        return *static_cast<const LocalVariable *>(ptr);
    }

    const GlobalVariableInfo &asGlobal() const {
        assert(kind == Kind::Global);
        return *static_cast<const GlobalVariableInfo *>(ptr);
    }

private:
    Kind kind;
    const void *ptr;
};

class Scope {

public:
    virtual ~Scope() = default;

    virtual const as::Type &resolveType(ast::Type &node) const = 0;
    virtual LocalVariable &createLocalVariable(String::Ref name, SourceLocation location, const as::Type &type) = 0;
    virtual Symbol resolveSymbol(ast::Name &name) const = 0;

protected:
    Scope() = default;

private:
    Scope(const Scope &) = delete;
    Scope(Scope &&) = delete;
    Scope &operator=(const Scope &) = delete;
    Scope &operator=(Scope &&) = delete;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_SCOPE_H_
