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
/// \brief Definition of the semantic analyzer.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_ANALYZER_H_
#define INCLUDE_QORE_COMP_SEM_ANALYZER_H_

#include <map>
#include <string>
#include <vector>
#include "qore/core/Env.h"
#include "qore/comp/Context.h"
#include "qore/comp/ast/Declaration.h"
#include "qore/comp/ast/Script.h"
#include "qore/comp/sem/Builder.h"
#include "qore/comp/sem/NamespaceScope.h"
#include "qore/comp/sem/stmt/Statement.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Semantic analyzer.
 *
 * Contains the shared state of the analyzer (\ref Core), the root \ref NamespaceScope and provides useful functions.
 */
class Analyzer {

public:
    /**
     * \brief Constructor.
     * \param ctx the compiler context
     */
    explicit Analyzer(Context &ctx) : core(ctx), root(core, ctx.getEnv().getRootNamespace()) {
    }

    /**
     * \brief Processes a declaration that is lexically in the root namespace.
     * \param decl the AST node
     */
    void processDeclaration(ast::Declaration &decl) {
        root.processDeclaration(decl);
    }

    /**
     * \brief Returns the root namespace scope.
     * \return the root namespace scope
     */
    NamespaceScope &getRootNamespaceScope() {
        return root;
    }

    /**
     * \brief Performs the first pass of a statement analysis.
     * \param scope the scope of the statement
     * \param stmt the AST node of the statement
     * \return the temporary representation of the statement
     */
    Statement::Ptr doPass1(Scope &scope, ast::Statement &stmt);

    /**
     * \brief Performs the second pass of a statement analysis.
     * \param builder the code builder
     * \param stmt the temporary representation of the statement
     */
    void doPass2(Builder &builder, Statement &stmt);

    /**
     * \brief Returns the array of global variable and constant initializers collected during the analysis.
     * \return the array of the initializers
     */
    std::vector<Statement::Ptr> takeInitializers() {
        return core.takeInitializers();
    }

    /**
     * \brief Processes all pending declarations, emptying the internal queues.
     */
    void processPendingDeclarations();

    /**
     * \brief Analyzes a whole script, synthesizing a function from the global variable and constant initializers and
     * the top level statements.
     * \param ctx the compiler context
     * \param node the AST node representing the script
     * \return the synthesized function
     * \todo the function should be probably added to the root namespace under some name
     */
    static Function::Ptr analyze(Context &ctx, ast::Script &node);

private:
    Core core;
    NamespaceScope root;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_ANALYZER_H_
