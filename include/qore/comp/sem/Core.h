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
#ifndef INCLUDE_QORE_COMP_SEM_CORE_H_
#define INCLUDE_QORE_COMP_SEM_CORE_H_

#include <string>
#include <utility>
#include "qore/comp/Context.h"
#include "qore/comp/ast/Declaration.h"
#include "qore/comp/ast/Statement.h"
#include "qore/comp/ast/Type.h"
#include "qore/comp/as/GlobalVariable.h"
#include "qore/comp/as/StringLiteral.h"
#include "qore/comp/sem/BlockScope.h"

namespace qore {
namespace comp {
namespace sem {

class NamespaceScope;
class ClassScope;
class GlobalVariableInfo;
class FunctionOverloadPack;
class FunctionScope;

class Core {

public:
    virtual ~Core() = default;

    virtual const as::Type &resolveType(const NamespaceScope &scope, const ast::Type &node) const = 0;

    virtual as::GlobalVariable &createGlobalVariable(String::Ref name, SourceLocation location,
            const as::Type &type) = 0;

    virtual as::StringLiteral createStringLiteral(const std::string &value) = 0;

    virtual void addToQueue(ClassScope &c) = 0;
    virtual void addToQueue(GlobalVariableInfo &gv) = 0;
    virtual void addToQueue(FunctionOverloadPack &fp) = 0;
    virtual void addToQueue(BlockScope &f, ast::Statement &stmt) = 0;

    //XXX c might not be fully constructed here
    virtual std::pair<const as::Type *, const as::Type *> createClassTypes(const ClassScope &c) = 0;

protected:
    explicit Core(Context &ctx) : ctx(ctx) {
    }

private:
    Core(const Core &) = delete;
    Core(Core &&) = delete;
    Core &operator=(const Core &) = delete;
    Core &operator=(Core &&) = delete;

public:
    Context &ctx;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_CORE_H_
