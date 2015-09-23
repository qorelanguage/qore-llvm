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

namespace qore {
namespace analyzer {
namespace script {

template<typename EA = expr::Analyzer>
class Visitor : public ast::StatementVisitor {              //TODO topLevelCommand

public:
    Visitor(Scope &scope, qil::CodeBuilder &codeBuilder) : scope(scope), codeBuilder(codeBuilder) {
    }

    void visit(const ast::CompoundStatement *node) override {
        QORE_UNREACHABLE("Not implemented");
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

private:
    Scope &scope;
    qil::CodeBuilder &codeBuilder;
};

class ScriptScope : public Scope {

public:
    ScriptScope(qil::CodeBuilder &codeBuilder) : codeBuilder(codeBuilder) {
    }

    void close(const SourceLocation &location) {
        for (auto i = variables.rbegin(), e = variables.rend(); i != e; ++i) {
            codeBuilder.lifetimeEnd(location, i->get());
        }
    }

    qil::Variable *createLocalVariable(const std::string &name, const SourceRange &range) override {
        qil::Variable *&var = varLookup[name];
        if (var) {
            //TODO error redeclaration
        } else {
            var = create(name, range);
            codeBuilder.lifetimeStart(range.start, var);
        }
        return var;
    }

    qil::Variable *resolve(const std::string &name, const SourceRange &range) override {
        qil::Variable *&var = varLookup[name];
        if (!var) {
            //TODO ERROR: undeclared
            var = create(name, range);
        }
        return var;
    }

    qil::StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) override {
        qil::StringLiteral *&s = strLookup[value];
        if (!s) {
            s = new qil::StringLiteral(std::move(value), range.start);
            strings.emplace_back(s);
        }
        return s;
    }

    std::vector<std::unique_ptr<qil::Variable>> getVariables() {
        return std::move(variables);
    }

    std::vector<std::unique_ptr<qil::StringLiteral>> getStrings() {
        return std::move(strings);
    }

private:
    qil::Variable *create(const std::string &name, const SourceRange &range) {
        qil::Variable *v = new qil::Variable(name, range.start);
        variables.emplace_back(v);
        return v;
    }

private:
    std::vector<std::unique_ptr<qil::Variable>> variables;
    std::map<std::string, qil::Variable *> varLookup;
    std::vector<std::unique_ptr<qil::StringLiteral>> strings;
    std::map<std::string, qil::StringLiteral *> strLookup;
    qil::CodeBuilder &codeBuilder;
};

class Analyzer {

public:
    template<typename EA = expr::Analyzer>
    Script analyze(const ast::Program::Ptr &node) {
        qil::CodeBuilder codeBuilder;
        ScriptScope scope(codeBuilder);
        Visitor<EA> visitor(scope, codeBuilder);
        for (const auto &stmt : node->body) {       //TODO stmt -> topLevelCommand
            stmt->accept(visitor);
        }
        scope.close(node->getRange().end);
        return Script(scope.getStrings(), scope.getVariables(), codeBuilder.build());
    }
};

} // namespace script
} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_SCRIPT_ANALYZER_H_
