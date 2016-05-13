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
/// \brief Defines the FunctionScope class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_FUNCTIONSCOPE_H_
#define INCLUDE_QORE_COMP_SEM_FUNCTIONSCOPE_H_

#include <map>
#include <string>
#include <vector>
#include "qore/core/Function.h"
#include "qore/comp/ast/Routine.h"
#include "qore/comp/sem/Scope.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Describes a function during semantic analysis and implements its scope.
 */
class FunctionScope : public Scope {

public:
    using Ptr = std::unique_ptr<FunctionScope>;     //!< Pointer type.

public:
    /**
     * \brief Constructor.
     * \param rt the runtime object representing the function
     * \param core the shared state of the analyzer
     * \param parent the parent namespace scope
     * \param node the AST node
     */
    FunctionScope(Function &rt, Core &core, Scope &parent, ast::Routine &node);

    const Type &resolveType(const ast::Type &node) const override;

    LocalVariableInfo &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) override;

    Symbol resolveSymbol(const ast::Name &name) const override;

    LocalVariableInfo &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        QORE_UNREACHABLE("");
    }

    const Type &getReturnType() const override {
        return rt.getType().getReturnType();
    }

    /**
     * \brief Performs the analysis of the function's body, i.e. translation to the \ref qore::code representation.
     * \param initializers optional initializer statements to prepend before the function's body
     */
    void analyze(std::vector<Statement::Ptr> *initializers = nullptr);

private:
    Function &rt;
    Core &core;
    Scope &parent;
    ast::Routine &node;
    std::vector<LocalVariableInfo::Ptr> locals;
    std::map<String::Ref, LocalVariableInfo *> args;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_FUNCTIONSCOPE_H_
