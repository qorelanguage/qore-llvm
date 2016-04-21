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

    const as::Type &resolveType(ast::Type &node) const override {
        return rootScope.resolveType(node);
    }

    LocalVariable &createLocalVariable(String::Ref name, SourceLocation location, const as::Type &type) override {
        std::unique_ptr<LocalVariable> ptr = util::make_unique<LocalVariable>(type);
        LocalVariable &lv = *ptr;
        locals.push_back(std::move(ptr));
        //lv.setShared();
        return lv;
    }

    Symbol resolveSymbol(ast::Name &name) const override {
        return rootScope.resolveSymbol(name);
    }

    LocalVariable &declareLocalVariable(String::Ref name, SourceLocation location, const as::Type &type) override {
        QORE_UNREACHABLE("");
    }

    const as::Type &getReturnType() const override {
        return as::Type::Nothing;
    }

private:
    const Scope &rootScope;
    std::vector<std::unique_ptr<LocalVariable>> locals;
};

class InteractiveFunctionBuilder : public Builder {

public:
    InteractiveFunctionBuilder() {
    }

    as::LocalSlot createLocalSlot() override {
        Id id = locals.size();
        locals.resize(id + 1);
        return as::LocalSlot(id);
    }

    as::Temp createTemp() override {
        Id id = temps.size();
        temps.resize(id + 1);
        return as::Temp(id);
    }

    void setLocal(as::LocalSlot slot, rt::qvalue value) {
        locals[slot.getIndex()] = value;
    }

    rt::qvalue getLocal(as::LocalSlot slot) {
        return locals[slot.getIndex()];
    }

    void setTemp(as::Temp temp, rt::qvalue value) {
        temps[temp.getIndex()] = value;
    }

    rt::qvalue getTemp(as::Temp temp) {
        return temps[temp.getIndex()];
    }

private:
    std::vector<rt::qvalue> temps;
    std::vector<rt::qvalue> locals;
};

class Interactive {

public:
    explicit Interactive(Context &compCtx) : compCtx(compCtx) {
    }

    void doIt() {
        StdinWrapper dp(compCtx);
        Parser parser(compCtx, dp);
        InteractiveFunctionBuilder mainBuilder;
        Core analyzer(compCtx);
        NamespaceScope root(analyzer);
        InteractiveScope topScope(root);
        BlockScopeImpl blockScope(topScope);
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
                execute(mainBuilder);
            } else {
                break;
            }
        }
        mainBuilder.popCleanupScopes();
        execute(mainBuilder);
    }

private:
    void execute(InteractiveFunctionBuilder &builder) {
        builder.createRetVoid();
        in::FunctionInterpreter<InteractiveFunctionBuilder> fi(rtCtx, builder);
        fi.run(builder.getBlock(0));
        builder.clear();
    }

private:
    Context &compCtx;
    rt::Context rtCtx;
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
