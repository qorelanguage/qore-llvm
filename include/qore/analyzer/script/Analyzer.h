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
/// \brief TODO File description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_ANALYZER_SCRIPT_ANALYZER_H_
#define INCLUDE_QORE_ANALYZER_SCRIPT_ANALYZER_H_

#include "qore/common/Util.h"
#include "qore/analyzer/expr/Analyzer.h"
#include <string>
#include <map>
#include <memory>
#include "qore/qil/CodeBuilder.h"
#include "qore/qil/ScriptBuilder.h"

namespace qore {
namespace analyzer {
namespace script {

class BlockScope : public Scope {

public:
    BlockScope(Scope &parentScope, qil::CodeBuilder &codeBuilder) : parentScope(parentScope), codeBuilder(codeBuilder) {
    }
    void close(const SourceLocation &location) {
        for (auto i = variables.rbegin(), e = variables.rend(); i != e; ++i) {
            codeBuilder.lifetimeEnd(location, *i);
        }
    }
    qil::Variable *declareVariable(const std::string &name, const SourceRange &range) override {
        qil::Variable *&var = varLookup[name];
        if (var) {
            QORE_UNREACHABLE("Redeclaration " << name);
            //TODO error redeclaration
        } else {
            var = createVariable(name, range);
            variables.push_back(var);
            codeBuilder.lifetimeStart(range.start, var);
        }
        return var;
    }
    qil::Variable *resolve(const std::string &name, const SourceRange &range) override {
        qil::Variable *&var = varLookup[name];
        if (!var) {
            return parentScope.resolve(name, range);
        }
        return var;
    }
    qil::StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) override {
        return parentScope.createStringLiteral(value, range);
    }

    qil::Variable *createVariable(const std::string &name, const SourceRange &range) override {
        return parentScope.createVariable(name, range);
    }

private:
    Scope &parentScope;
    qil::CodeBuilder &codeBuilder;
    std::vector<qil::Variable *> variables;
    std::map<std::string, qil::Variable *> varLookup;
};

template<typename EA = expr::Analyzer>
class Visitor : public ast::StatementVisitor {              //TODO topLevelCommand

public:
    Visitor(Scope &scope, qil::CodeBuilder &codeBuilder) : scope(scope), codeBuilder(codeBuilder) {
    }

    void visit(const ast::CompoundStatement *node) override {
        BlockScope blockScope(scope, codeBuilder);
        analyze(node->statements, blockScope, codeBuilder);
        blockScope.close(node->getRange().end);
    }

    void visit(const ast::IfStatement *node) override {
        EA().evalValue(scope, codeBuilder, node->condition);

        qil::BasicBlock *thenBlock = codeBuilder.createBasicBlock();
        qil::BasicBlock *elseBlock = codeBuilder.createBasicBlock();        //TODO empty else branch -> no BB needed
        qil::BasicBlock *mergeBlock = codeBuilder.createBasicBlock();

        codeBuilder.terminateWithConditional(thenBlock, elseBlock);

        codeBuilder.setBasicBlock(thenBlock);
        node->thenBranch->accept(*this);
        codeBuilder.terminate(mergeBlock);

        codeBuilder.setBasicBlock(elseBlock);
        node->elseBranch->accept(*this);
        codeBuilder.terminate(mergeBlock);

        codeBuilder.setBasicBlock(mergeBlock);
    }

    void visit(const ast::EmptyStatement *node) override {
    }

    void visit(const ast::ExpressionStatement *node) override {
        EA().eval(scope, codeBuilder, node->expression);
    }

    void visit(const ast::PrintStatement *node) override {
        EA().evalValue(scope, codeBuilder, node->expression);
        codeBuilder.print(node->getRange().start);
    }

public:
    static void analyze(const ast::Statements &statements, Scope &scope, qil::CodeBuilder &codeBuilder) {
        Visitor<EA> visitor(scope, codeBuilder);
        for (const auto &stmt : statements) {       //TODO stmt -> topLevelCommand
            stmt->accept(visitor);
        }
    }

private:
    Scope &scope;
    qil::CodeBuilder &codeBuilder;
};

class ScriptScope : public Scope {

public:
    ScriptScope(qil::ScriptBuilder &scriptBuilder) : scriptBuilder(scriptBuilder) {
    }

    qil::Variable *declareVariable(const std::string &name, const SourceRange &range) override {
        QORE_UNREACHABLE("Should not happen");
    }

    qil::Variable *resolve(const std::string &name, const SourceRange &range) override {
        //TODO recovery must be handled by the caller, otherwise the variable will always be global
        QORE_UNREACHABLE("Undeclared " << name);
    }

    qil::StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) override {
        return scriptBuilder.createStringLiteral(value, range);
    }

    qil::Variable *createVariable(const std::string &name, const SourceRange &range) override {
        return scriptBuilder.createVariable(name, range);
    }

private:
    qil::ScriptBuilder &scriptBuilder;
};

class Analyzer {

public:
    template<typename EA = expr::Analyzer>
    qil::Script analyze(const ast::Program::Ptr &node) {
        qil::ScriptBuilder scriptBuilder;
        ScriptScope scriptScope(scriptBuilder);
        BlockScope scope(scriptScope, scriptBuilder.getCodeBuilder());
        Visitor<EA>::analyze(node->body, scope, scriptBuilder.getCodeBuilder());
        scope.close(node->getRange().end);
        scriptBuilder.getCodeBuilder().terminateRetVoid();
        return scriptBuilder.build();
    }
};

} // namespace script
} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_SCRIPT_ANALYZER_H_
