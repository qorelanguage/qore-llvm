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
#include "qore/core/util/Debug.h"
#include "qore/comp/DirectiveProcessor.h"
#include "qore/comp/Parser.h"
#include "qore/comp/sem/Analyzer.h"
#include "qore/in/FunctionInterpreter.h"
#include "qore/core/Env.h"
#include "DiagPrinter.h"

namespace qore {

class StdinWrapper : public comp::ITokenStream {

public:
    explicit StdinWrapper(comp::Context &ctx)
            : src(ctx.getSrcMgr().createFromString(ctx.getEnv().addSourceInfo("<stdin>"), "")), dp(ctx, src) {
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

    const Type &resolveType(const ast::Type &node) const override {
        return rootScope.resolveType(node);
    }

    LocalVariableInfo &createLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        std::unique_ptr<LocalVariableInfo> ptr = util::make_unique<LocalVariableInfo>(name, location, type);
        LocalVariableInfo &lv = *ptr;
        locals.push_back(std::move(ptr));
        //lv.setShared();
        return lv;
    }

    Symbol resolveSymbol(const ast::Name &name) const override {
        return rootScope.resolveSymbol(name);
    }

    LocalVariableInfo &declareLocalVariable(String::Ref name, SourceLocation location, const Type &type) override {
        QORE_UNREACHABLE("");
    }

    const Type &getReturnType() const override {
        return Type::Nothing;
    }

private:
    const Scope &rootScope;
    std::vector<std::unique_ptr<LocalVariableInfo>> locals;
};

class TopLevelCtx {

public:
    void setLocal(const LocalVariable &lv, qvalue value) {
        locals[lv.getIndex()] = value;
    }

    qvalue getLocal(const LocalVariable &lv) {
        auto it = locals.find(lv.getIndex());
        assert(it != locals.end());
        return it->second;
    }

    void setTemp(code::Temp temp, qvalue value) {
        temps[temp.getIndex()] = value;
    }

    qvalue getTemp(code::Temp temp) {
        auto it = temps.find(temp.getIndex());
        assert(it != temps.end());
        return it->second;
    }

private:
    std::map<Index, qvalue> temps;
    std::map<Index, qvalue> locals;
};

//XXX: this class can work with TopLevelCtx::temps and locals -> no need for map
class IBuilder : public Builder {

public:
    explicit IBuilder(Context &ctx) : Builder(ctx), tempCount(0) {
        currentBlock = entry = createBlock();
    }

    code::Temp createTemp() override {
        return code::Temp(tempCount++);
    }

    LocalVariable &createLocalVariable(std::string name, const Type &type, SourceLocation location) override {
        LocalVariable::Ptr ptr = util::make_unique<LocalVariable>(locals.size(), std::move(name), type, location);
        LocalVariable &v = *ptr;
        locals.push_back(std::move(ptr));
        return v;
    }

    code::Block *createBlock() override {
        code::Block::Ptr ptr = code::Block::Ptr(new code::Block());
        code::Block *b = ptr.get();
        blocks.push_back(std::move(ptr));
        return b;
    }

    code::Block &getEntryForInteractiveMode() {
        currentBlock->appendRetVoid();
        code::Block *b = entry;
        currentBlock = entry = createBlock();
        return *b;
    }

private:
    Size tempCount;
    std::vector<LocalVariable::Ptr> locals;
    std::vector<code::Block::Ptr> blocks;
    code::Block *entry;
};

class Interactive {

public:
    explicit Interactive(Context &compCtx) : compCtx(compCtx) {
    }

    void doIt() {
        StdinWrapper dp(compCtx);
        Parser parser(compCtx, dp);
        IBuilder mainBuilder(compCtx);
        Core analyzer(compCtx);
        NamespaceScope root(analyzer, compCtx.getEnv().getRootNamespace());
        InteractiveScope topScope(root);
        BlockScopeImpl blockScope(topScope);
        TopLevelCtx topLevelCtx;
        while (true) {
            comp::DeclOrStmt declOrStmt = parser.parseDeclOrStmt();
            if (declOrStmt.isDeclaration()) {
                root.processDeclaration(declOrStmt.getDeclaration());
                analyzer.processPendingDeclarations();
            } else if (declOrStmt.isStatement()) {
                Statement::Ptr stmt = analyzer.doPass1(blockScope, declOrStmt.getStatement());
                auto initializers = analyzer.takeInitializers();
                for (auto &stmt : initializers) {
                    analyzer.doPass2(mainBuilder, *stmt);
                }
                analyzer.doPass2(mainBuilder, *stmt);
                in::FunctionInterpreter<TopLevelCtx> fi(topLevelCtx, mainBuilder.getEntryForInteractiveMode());
                fi.run();
            } else {
                break;
            }
        }
        mainBuilder.popCleanupScopes();
        in::FunctionInterpreter<TopLevelCtx> fi(topLevelCtx, mainBuilder.getEntryForInteractiveMode());
        fi.run();
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
    DiagPrinter diagPrinter;
    diagMgr.addProcessor(&diagPrinter);
    Env env;
    comp::Context ctx(env, stringTable, diagMgr, srcMgr);
    comp::sem::Interactive i(ctx);
    i.doIt();
}

} // namespace qore
