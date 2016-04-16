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

#include <qore/comp/sem/Core.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "qore/comp/Context.h"
#include "qore/comp/as/Script.h"
#include "qore/comp/ast/Declaration.h"
#include "qore/comp/sem/NamespaceScope.h"
#include "qore/comp/sem/stmt/Statement.h"
#include "qore/comp/sem/Builder.h"
#include "qore/comp/ast/Script.h"

namespace qore {
namespace comp {
namespace sem {


class AnalyzerCallbacks {

public:
    virtual ~AnalyzerCallbacks() = default;

    /*
     * - interactive mode passes it through pass2 and runs it directly using interpreter
     * - normal mode puts it in qinit
     */
    virtual void addInitializer(Statement::Ptr stmt) = 0;
};

class Analyzer : private Core {

public:
    Analyzer(Context &ctx, AnalyzerCallbacks &callbacks, as::Script &script);

    void processDeclaration(ast::Declaration &decl);

    void processPendingDeclarations();

    /*
     * transforms ast to ir (it does type checking, resolves all symbols and operators, decides
     * which local variables need to be shared), adds closures to the queue (?)
     */
    Statement::Ptr doPass1(BlockScope &scope, ast::Statement &stmt);

    /*
     * transforms ir to as (exception safety, temporaries, ...)
     */
    void doPass2(Builder &builder, Statement &stmt);

    //void finalizeScope ???

    const NamespaceScope &getRootNamespace() {
        return rootNamespace;
    }

private:
    void addToQueue(ClassScope &c) override {
        classQueue.push_back(&c);
    }

    void addToQueue(GlobalVariableInfo &gv) override {
        globalVariableQueue.push_back(&gv);
    }

    void addToQueue(FunctionOverloadPack &fp) override {
        functionOverloadPackQueue.push_back(&fp);
    }

    void addToQueue(BlockScope &f, ast::Statement &stmt) override {
        functionQueue.emplace_back(&f, &stmt);
    }

    const as::Type &resolveType(const NamespaceScope &scope, const ast::Type &node) const override;

    as::GlobalVariable &createGlobalVariable(String::Ref name, SourceLocation location, const as::Type &type) override;

    as::StringLiteral createStringLiteral(const std::string &value) override;

    std::pair<const as::Type *, const as::Type *> createClassTypes(const ClassScope &c) override {
        const as::Type &t1 = script.createType(rt::Type::Object, c.getFullName(), false);
        const as::Type &t2 = script.createType(rt::Type::Object, c.getFullName(), true);
        return std::make_pair(&t1, &t2);
    }

private:
    AnalyzerCallbacks &callbacks;
    as::Script &script;
    NamespaceScope rootNamespace;
    std::vector<ClassScope *> classQueue;
    std::vector<GlobalVariableInfo *> globalVariableQueue;
    std::vector<FunctionOverloadPack *> functionOverloadPackQueue;
    std::vector<std::pair<BlockScope *, ast::Statement *>> functionQueue;
    std::map<String::Ref, std::pair<const as::Type *, const as::Type *>> builtinTypes;
    std::unordered_map<std::string, as::StringLiteral> strings;

    template<typename OS> friend void dump(OS, Analyzer &);
};

class ACB : public AnalyzerCallbacks {

public:
    void addInitializer(Statement::Ptr stmt) override {
        initializers.push_back(std::move(stmt));
    }

    std::vector<Statement::Ptr> initializers;
};

class QMS : public Scope {      //temporary - simulates the scope of qmain

public:
    explicit QMS(const Scope &rootScope) : rootScope(rootScope) {
    }

    const as::Type &resolveType(ast::Type &node) const override {
        return rootScope.resolveType(node);
    }

    LocalVariable &createLocalVariable(String::Ref name, SourceLocation location, const as::Type &type) override {
        std::unique_ptr<LocalVariable> ptr = util::make_unique<LocalVariable>(type);
        LocalVariable &lv = *ptr;
        locals.push_back(std::move(ptr));
        return lv;
    }

    Symbol resolveSymbol(ast::Name &name) const override {
        return rootScope.resolveSymbol(name);
    }

private:
    const Scope &rootScope;
    std::vector<std::unique_ptr<LocalVariable>> locals;
};


inline as::Script::Ptr analyze(Context &ctx, ast::Script &node) {
    as::Script::Ptr script = util::make_unique<as::Script>();

    ACB cb;
    Analyzer analyzer(ctx, cb, *script);

    for (auto &decl : node.members) {
        analyzer.processDeclaration(*decl);
    }
    analyzer.processPendingDeclarations();

    //once we support functions, synthesize qmain and put it into funcQueue before calling processPendingDeclarations();
    QMS qms(analyzer.getRootNamespace());
    BlockScopeImpl topLevelScope(qms);

    std::vector<Statement::Ptr> temp;
    for (auto &stmt : node.statements) {
        temp.push_back(analyzer.doPass1(topLevelScope, *stmt));
    }
    temp.push_back(topLevelScope.finalize());

    FunctionBuilder mainBuilder("qmain");
    for (auto &stmt : temp) {
        analyzer.doPass2(mainBuilder, *stmt);
    }
    mainBuilder.createRetVoid();
    mainBuilder.build(*script);

    //qinit - pass2
    FunctionBuilder initBuilder("qinit");
    for (auto &stmt : cb.initializers) {
        analyzer.doPass2(initBuilder, *stmt);
    }
    initBuilder.createRetVoid();
    initBuilder.build(*script);

    return script;
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_ANALYZER_H_
