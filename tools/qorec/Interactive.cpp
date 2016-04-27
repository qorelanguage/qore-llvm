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
#include "Interactive.h"
#include <map>
#include <string>
#include <vector>
#include "qore/common/Logging.h"
#include "qore/comp/DirectiveProcessor.h"
#include "qore/comp/Parser.h"
#include "qore/comp/sem/Analyzer.h"
#include "qore/in/FunctionInterpreter.h"
#include "qore/rt/Context.h"
#include "DiagPrinter.h"

namespace qore {

class StdinWrapper : public comp::ITokenStream {

public:
    explicit StdinWrapper(comp::Context &ctx)
            : src(ctx.getSrcMgr().createFromString("<stdin>", "")), dp(ctx, src) {
    }

    comp::Token read(Mode mode) override {
        while (true) {
            comp::Token t = dp.read(mode);
            if (t.type != comp::TokenType::EndOfFile) {
                return t;
            }
            std::string line;
            std::getline(std::cin, line);
            if (!std::cin.good()) {
                return t;
            }
            line.push_back('\n');
            src.append(line);
        }
    }

private:
    comp::Source &src;
    comp::DirectiveProcessor dp;
};

namespace comp {
namespace sem {

class InteractiveScope : public Scope {

public:
    explicit InteractiveScope(const Scope &rootScope) : rootScope(rootScope) {
    }

    const Type &resolveType(ast::Type &node) const override {
        return rootScope.resolveType(node);
    }

    LocalVariable &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        std::unique_ptr<LocalVariable> ptr = util::make_unique<LocalVariable>(name, location, type);
        LocalVariable &lv = *ptr;
        locals.push_back(std::move(ptr));
        //lv.setShared();
        return lv;
    }

    Symbol resolveSymbol(ast::Name &name) const override {
        return rootScope.resolveSymbol(name);
    }

    LocalVariable &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        QORE_UNREACHABLE("");
    }

    const Type &getReturnType() const override {
        return Type::Nothing;
    }

private:
    const Scope &rootScope;
    std::vector<std::unique_ptr<LocalVariable>> locals;
};

class TopLevelCtx {

public:
    void setLocal(const as::LocalVariable &lv, qvalue value) {
        locals[lv.getId()] = value;
    }

    qvalue getLocal(const as::LocalVariable &lv) {
        auto it = locals.find(lv.getId());
        assert(it != locals.end());
        return it->second;
    }

    void setTemp(as::Temp temp, qvalue value) {
        temps[temp.getIndex()] = value;
    }

    qvalue getTemp(as::Temp temp) {
        auto it = temps.find(temp.getIndex());
        assert(it != temps.end());
        return it->second;
    }

private:
    std::map<Id, qvalue> temps;
    std::map<Id, qvalue> locals;
};

class Interactive {

public:
    explicit Interactive(Context &compCtx) : compCtx(compCtx) {
    }

    void doIt() {
        rt::Context rtCtx;
        StdinWrapper dp(compCtx);
        Parser parser(compCtx, dp);
        Builder mainBuilder;
        Core analyzer(compCtx);
        NamespaceScope root(analyzer);
        InteractiveScope topScope(root);
        BlockScopeImpl blockScope(topScope);
        TopLevelCtx topLevelCtx;
        while (true) {
            comp::Parser::DeclOrStmt declOrStmt = parser.parseDeclOrStmt();
            if (declOrStmt.decl) {
                root.processDeclaration(*declOrStmt.decl);
                analyzer.processPendingDeclarations();
            } else if (declOrStmt.stmt) {
                Statement::Ptr stmt = analyzer.doPass1(blockScope, *declOrStmt.stmt);
                auto initializers = analyzer.scriptBuilder.takeInitializers();
                for (auto &stmt : initializers) {
                    analyzer.doPass2(mainBuilder, *stmt);
                }
                analyzer.doPass2(mainBuilder, *stmt);
                in::FunctionInterpreter<TopLevelCtx> fi(rtCtx, topLevelCtx);
                fi.run(mainBuilder.getEntryForInteractiveMode());
            } else {
                break;
            }
        }
        mainBuilder.popCleanupScopes();
        in::FunctionInterpreter<TopLevelCtx> fi(rtCtx, topLevelCtx);
        fi.run(mainBuilder.getEntryForInteractiveMode());
    }

private:
    Context &compCtx;
};

} // namespace sem
} // namespace comp

void interactive() {
    LOG_FUNCTION();

    comp::StringTable stringTable;
    comp::DiagManager diagMgr(stringTable);
    comp::SourceManager srcMgr(diagMgr);
    DiagPrinter diagPrinter(srcMgr);
    diagMgr.addProcessor(&diagPrinter);
    comp::Context ctx(stringTable, diagMgr, srcMgr);
    comp::sem::Interactive i(ctx);
    i.doIt();
}

} // namespace qore
