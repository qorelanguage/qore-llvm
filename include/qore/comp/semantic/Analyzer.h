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
/// \brief Semantic analyzer.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_ANALYZER_H_
#define INCLUDE_QORE_COMP_SEMANTIC_ANALYZER_H_

#include <utility>
#include <vector>
#include "qore/comp/ast/Script.h"
#include "qore/comp/ast/Class.h"
#include "qore/comp/Context.h"
#include "qore/comp/semantic/Namespace.h"
#include "qore/comp/semantic/TypeRegistry.h"
#include "qore/common/Util.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Semantic analyzer.
 */
class Analyzer {

public:
    /**
     * \brief Constructs an instance.
     * \param ctx the compiler context
     */
    explicit Analyzer(Context &ctx) : ctx(ctx), typeRegistry(ctx) {
    }

    /**
     * \brief Analyzes the script.
     * \param script the script to analyze
     * \return the root namespace
     */
    std::unique_ptr<Namespace> analyze(ast::Script::Ptr &script) {
        std::unique_ptr<Namespace> root = util::make_unique<Namespace>(ctx);
        collectNamespacesAndClasses(*root, script->members);
        declareNamespaceMembers();
        processClasses();
        return root;
    }

private:
    void collectNamespacesAndClasses(Namespace &ns, std::vector<ast::Declaration::Ptr> &declNodes);
    void declareNamespaceMembers();
    void processClasses();
    void createNamespace(Namespace &scope, ast::Namespace &node);
    void createClass(Namespace &scope, ast::Class &node);
    void createGlobalVariable(Namespace &scope, ast::GlobalVariable &node);
    Namespace &findParentFor(Namespace &scope, const ast::Name &name);

    Type::Ref resolveType(Scope &scope, const ast::Type &node) {
        return typeRegistry.resolve(scope, node);
    }

private:
    Context &ctx;
    TypeRegistry typeRegistry;

    std::vector<std::pair<Namespace *, ast::Declaration *>> nsMemberQueue;
    std::vector<std::pair<Class *, ast::Class *>> classQueue;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_ANALYZER_H_
