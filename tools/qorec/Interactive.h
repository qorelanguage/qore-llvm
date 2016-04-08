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
#ifndef TOOLS_QOREC_INTERACTIVE_H_
#define TOOLS_QOREC_INTERACTIVE_H_

#include <string>
#include <unordered_map>
#include <vector>
#include "qore/comp/sem/Analyzer.h"
#include "qore/in/Interpreter.h"
#include "qore/rt/Func.h"
#include "DiagPrinter.h"

namespace qore {

class StdinWrapper : public qore::comp::ITokenStream {

public:
    explicit StdinWrapper(qore::comp::Context &ctx)
            : src(ctx.getSrcMgr().createFromString("<stdin>", "")), dp(ctx, src) {
    }

    qore::comp::Token read(Mode mode) override {
        while (true) {
            qore::comp::Token t = dp.read(mode);
            if (t.type != qore::comp::TokenType::EndOfFile) {
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
    qore::comp::Source &src;
    qore::comp::DirectiveProcessor dp;
};


class InteractiveGlobalScope : public comp::sem::GlobalScope {

public:
    InteractiveGlobalScope(comp::Context &ctx, ir::Script &script, in::Interpreter &interpreter)
            : GlobalScope(ctx, script), interpreter(interpreter) {
    }

    const ir::StringLiteral &createStringLiteral(const std::string &value) override {
        const qore::ir::StringLiteral &sl = GlobalScope::createStringLiteral(value);
        interpreter.addStringLiteral(sl);
        return sl;
    }

    comp::sem::Symbol declareGlobalVariable(comp::ast::Name &name, const ir::Type &type) override {
        comp::sem::Symbol gv = GlobalScope::declareGlobalVariable(name, type);
        interpreter.addGlobalVariable(gv.asGlobal());
        return gv;
    }

public:
    in::Interpreter &interpreter;
};

class TopLevelScope : public comp::sem::BlockScope {

public:
    TopLevelScope(comp::Context &ctx, Scope &parent) : ctx(ctx), parent(parent) {
    }

    ~TopLevelScope() {
        for (auto &lv : topLevelLocals) {
            if (!lv->getType().isPrimitive()) {
                rt::qvalue v = locals.get(*lv);
                rt::decRef(v);      //TODO exceptions
            }
        }
    }

    const ir::Type &resolveType(comp::ast::Type &node) override {
        return parent.resolveType(node);
    }

    comp::sem::Symbol declareLocalVariable(comp::String::Ref name, const ir::Type &type) override {
        std::string n = ctx.getStringTable().get(name);

        ir::LocalVariable::Ptr ptr = ir::LocalVariable::create(n, type);
        const ir::LocalVariable &lv = *ptr;
        topLevelLocals.push_back(std::move(ptr));
        locals.add(lv);
        comp::sem::Symbol s = comp::sem::Symbol(lv);
        symbols[n] = s;
        return s;
    }

    comp::sem::Symbol resolve(comp::ast::Name &name) override {
        if (name.isSimple()) {
            const std::string &n = ctx.getStringTable().get(name.last().str);
            auto it = symbols.find(n);
            if (it != symbols.end()) {
                return it->second;
            }
        }
        return parent.resolve(name);
    }

    const ir::StringLiteral &createStringLiteral(const std::string &value) override {
        return parent.createStringLiteral(value);
    }

private:
    comp::Context &ctx;
    Scope &parent;
    std::unordered_map<std::string, comp::sem::Symbol> symbols;
    std::vector<ir::LocalVariable::Ptr> topLevelLocals;

public:
    in::Locals locals;
};

void interactive() {
    LOG_FUNCTION();

    comp::StringTable stringTable;
    comp::DiagManager diagMgr(stringTable);
    comp::SourceManager srcMgr(diagMgr);
    DiagPrinter diagPrinter(srcMgr);
    diagMgr.addProcessor(&diagPrinter);
    comp::Context ctx(stringTable, diagMgr, srcMgr);
    StdinWrapper dp(ctx);

    comp::Parser parser(ctx, dp);

    ir::Script script;
    comp::sem::Analyzer a(ctx);
    in::Interpreter interpreter;      //XXX interpreter has ptrs to script which it uses in dtor
    InteractiveGlobalScope globalScope(ctx, script, interpreter);
    TopLevelScope topLevelScope(ctx, globalScope);

    while (true) {
        comp::Parser::DeclOrStmt declOrStmt = parser.parseDeclOrStmt();
        if (declOrStmt.decl) {
            a.doDecl(*declOrStmt.decl, globalScope);
        } else if (declOrStmt.stmt) {
            a.doStmt(*declOrStmt.stmt, topLevelScope, [&interpreter, &topLevelScope](ir::Statement::Ptr stmt){
                interpreter.execute(topLevelScope.locals, *stmt);
            });
        } else {
            break;
        }
    }
}

} // namespace qore

#endif // TOOLS_QOREC_INTERACTIVE_H_
