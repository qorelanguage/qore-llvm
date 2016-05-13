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
/// \brief Interactive mode implementation.
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
#include "qore/comp/sem/BlockScope.h"
#include "qore/in/FunctionInterpreter.h"
#include "qore/core/Env.h"
#include "DiagPrinter.h"

namespace qore {

///\cond
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

class InteractiveScope : public comp::sem::Scope {

private:
    using LocalVariableInfo = comp::sem::LocalVariableInfo;

public:
    InteractiveScope(comp::Context &ctx, const Scope &rootScope, in::FunctionContext &topLevelCtx)
            : ctx(ctx), rootScope(rootScope), topLevelCtx(topLevelCtx) {
    }

    const Type &resolveType(const comp::ast::Type &node) const override {
        return rootScope.resolveType(node);
    }

    LocalVariableInfo &createLocalVariable(comp::String::Ref name, SourceLocation location, const Type &type) override {
        LocalVariableInfo::Ptr ptr = LocalVariableInfo::Ptr(new LocalVariableInfo(name, location, type));
        LocalVariableInfo &lv = *ptr;
        localInfos.push_back(std::move(ptr));
        //lv.setShared();
        LocalVariable::Ptr ptr2 = LocalVariable::Ptr(new LocalVariable(locals.size(),
                ctx.getString(name), type, location));
        lv.setRt(*ptr2);
        locals.push_back(std::move(ptr2));
        topLevelCtx.locals.resize(locals.size());
        return lv;
    }

    comp::sem::Symbol resolveSymbol(const comp::ast::Name &name) const override {
        return rootScope.resolveSymbol(name);
    }

    LocalVariableInfo &declareLocalVariable(comp::String::Ref name,
            SourceLocation location, const Type &type) override {
        QORE_UNREACHABLE("");
    }

    const Type &getReturnType() const override {
        return Type::Nothing;
    }

private:
    comp::Context &ctx;
    const Scope &rootScope;
    in::FunctionContext &topLevelCtx;
    std::vector<LocalVariableInfo::Ptr> localInfos;
    std::vector<LocalVariable::Ptr> locals;
};

class IBuilder : public comp::sem::Builder {

public:
    explicit IBuilder(in::FunctionContext &topLevelCtx) : topLevelCtx(topLevelCtx) {
        entry = createBlock();
        setCurrentBlock(entry);
    }

    code::Temp createTemp() override {
        Index i = topLevelCtx.temps.size();
        topLevelCtx.temps.resize(i + 1);
        return code::Temp(i);
    }

    code::Block *createBlock() override {
        code::Block::Ptr ptr = code::Block::Ptr(new code::Block());
        code::Block *b = ptr.get();
        blocks.push_back(std::move(ptr));
        return b;
    }

    code::Block &getEntryForInteractiveMode() {
        createRetVoidInteractive();
        code::Block *b = entry;
        entry = createBlock();
        setCurrentBlock(entry);
        return *b;
    }

private:
    in::FunctionContext &topLevelCtx;
    std::vector<code::Block::Ptr> blocks;
    code::Block *entry;
};
///\endcond

void interactive() {
    LOG_FUNCTION();

    comp::StringTable stringTable;
    comp::DiagManager diagMgr(stringTable);
    comp::SourceManager srcMgr(diagMgr);
    DiagPrinter diagPrinter;
    diagMgr.addProcessor(&diagPrinter);
    Env env;
    comp::Context ctx(env, stringTable, diagMgr, srcMgr);

    StdinWrapper dp(ctx);
    comp::Parser parser(ctx, dp);
    in::FunctionContext topLevelCtx(0, 0);
    IBuilder mainBuilder(topLevelCtx);
    comp::sem::Analyzer analyzer(ctx);
    InteractiveScope topScope(ctx, analyzer.getRootNamespaceScope(), topLevelCtx);
    comp::sem::BlockScope blockScope(topScope);
    {
        comp::sem::LocalsStackMarker marker(mainBuilder);
        while (true) {
            comp::DeclOrStmt declOrStmt = parser.parseDeclOrStmt();
            if (declOrStmt.isDeclaration()) {
                analyzer.processDeclaration(declOrStmt.getDeclaration());
                analyzer.processPendingDeclarations();
            } else if (declOrStmt.isStatement()) {
                comp::sem::Statement::Ptr stmt = analyzer.doPass1(blockScope, declOrStmt.getStatement());
                auto initializers = analyzer.takeInitializers();
                for (auto &stmt : initializers) {
                    analyzer.doPass2(mainBuilder, *stmt);
                }
                analyzer.doPass2(mainBuilder, *stmt);
                in::FunctionInterpreter fi(topLevelCtx, mainBuilder.getEntryForInteractiveMode());
                fi.run();
            } else {
                break;
            }
        }
    }
    in::FunctionInterpreter fi(topLevelCtx, mainBuilder.getEntryForInteractiveMode());
    fi.run();
}

} // namespace qore
