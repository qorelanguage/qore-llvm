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
#include "qore/ast/Program.h"
#include "qore/ast/Rewriter.h"
#include "qore/qil/Qil.h"
#include <string>
#include <map>
#include <memory>

namespace qore {
namespace analyzer {

class Scope {

public:
    virtual ~Scope() = default;

    virtual ast::Variable::Ptr declareVariable(const std::string &name, const SourceRange &range) = 0;
    virtual ast::Variable::Ptr resolve(const std::string &name, const SourceRange &range) = 0;

    virtual ast::Variable::Ptr createVariable(const std::string &name, const SourceRange &range) = 0;
};

class ScopeAdder : public ast::Rewriter {
public:
    ast::Statement::Ptr rewrite(ast::IfStatement::Ptr node) override {
        return ast::ScopedStatement::create(node);
    }

    ast::Statement::Ptr rewrite(ast::TryStatement::Ptr node) override {
        node->tryBody = ast::ScopedStatement::create(node->tryBody);
        //TODO try/catch scope
        return node;
    }

    ast::Statement::Ptr rewrite(ast::CompoundStatement::Ptr node) override {
        return ast::ScopedStatement::create(node);
    }
};

class BlockScope : public Scope {
public:
    BlockScope(Scope &parentScope) : parentScope(parentScope) {
    }
    void closeInteractive() {
        for (auto it = variables.rbegin(); it != variables.rend(); ++it) {
            QoreValue *qv = static_cast<QoreValue *>((*it)->data);
            strongDeref(*qv);
            delete qv;
        }
    }
    void close(ast::ScopedStatement::Ptr stmt) {
        stmt->variables = std::move(variables);
    }
    ast::Variable::Ptr declareVariable(const std::string &name, const SourceRange &range) override {
        ast::Variable::Ptr &var = varLookup[name];
        if (var) {
            QORE_UNREACHABLE("Redeclaration " << name);
            //TODO error redeclaration
        } else {
            var = createVariable(name, range);
            variables.push_back(var);
        }
        return var;
    }
    ast::Variable::Ptr resolve(const std::string &name, const SourceRange &range) override {
        ast::Variable::Ptr &var = varLookup[name];
        if (!var) {
            return parentScope.resolve(name, range);
        }
        return var;
    }

    ast::Variable::Ptr createVariable(const std::string &name, const SourceRange &range) override {
        return parentScope.createVariable(name, range);
    }

private:
    Scope &parentScope;
    std::vector<ast::Variable::Ptr> variables;
    std::map<std::string, ast::Variable::Ptr> varLookup;
};

class Resolver : public ast::Rewriter {
public:
    Resolver(Scope &scope) : scope(scope) {
    }

    ast::Expression::Ptr rewrite(ast::VarDecl::Ptr node) override {
        return scope.declareVariable(node->name, node->getRange());
    }

    ast::Expression::Ptr rewrite(ast::Identifier::Ptr node) override {
        return scope.resolve(node->name, node->getRange());
    }

    void visit(ast::ScopedStatement::Ptr node) override {
        BlockScope bs(scope);
        Resolver r(bs);
        r.recurse(node->statement);
        bs.close(node);
    }

private:
    Scope &scope;
};

class StringReplacer : public ast::Rewriter {
public:
    StringReplacer(bool interactive = false) : interactive(interactive) {
    }

    ~StringReplacer() {
        if (interactive) {
            for (auto it = strings.rbegin(); it != strings.rend(); ++it) {
                QoreValue *qv = static_cast<QoreValue *>((*it)->data);
                strongDeref(*qv);
                delete qv;
            }
        }
    }

    ast::Expression::Ptr rewrite(ast::StringLiteral::Ptr node) override {
        ast::StringConstant::Ptr &s = strLookup[node->value];
        if (!s) {
            s = ast::StringConstant::create(node->getRange(), std::move(node->value));
            if (interactive) {
                QoreValue *qv = new QoreValue();
                *qv = make_str(s->value.c_str());
                s->data = qv;
            }
            strings.emplace_back(s);
        }
        return s;
    }

public:
    qil::Script::Strings strings;
    std::map<std::string, ast::StringConstant::Ptr> strLookup;
private:
    bool interactive;
};

//class ConstFolder : public ast::Rewriter {
//public:
//    ConstFolder() {
//    }
//
//    ast::Expression *rewrite(ast::BinaryExpression *node) override {
//        //TODO remove dynamic casts
//        ast::StringLiteral *left = dynamic_cast<ast::StringLiteral *>(node->left.get());
//        ast::StringLiteral *right = dynamic_cast<ast::StringLiteral *>(node->right.get());
//        if (left && right) {
//            left->value += right->value;
//            node->left.release();
//            //delete node
//            return left;
//        }
//        return node;
//    }
//};

class ScriptScope : public Scope {

public:
    ScriptScope() {
    }

    ast::Variable::Ptr declareVariable(const std::string &name, const SourceRange &range) override {
        QORE_UNREACHABLE("Should not happen");
    }

    ast::Variable::Ptr resolve(const std::string &name, const SourceRange &range) override {
        //TODO recovery must be handled by the caller, otherwise the variable will always be global
        QORE_UNREACHABLE("Undeclared " << name);
    }

    ast::Variable::Ptr createVariable(const std::string &name, const SourceRange &range) override {
        ast::Variable::Ptr v = ast::Variable::create(range, name);
        initVar(v);
        variables.emplace_back(v);
        return v;
    }

    virtual void initVar(ast::Variable::Ptr &v) {
    }

public:
    qil::Script::Variables variables;
};

class InteractiveScriptScope : public ScriptScope {
public:
    virtual void initVar(ast::Variable::Ptr &v) {
        QoreValue *qv = new QoreValue();
        qv->tag = Tag::Nothing;
        v->data = qv;
    }
};


class Analyzer {

public:
    qil::Script analyze(const ast::Program::Ptr &node) {
        SourceRange range = node->getRange();
        ast::Statement::Ptr body = ast::CompoundStatement::create(range, std::move(node->body));

        ScopeAdder sa;
        sa.recurse(body);

        ScriptScope scriptScope;
        Resolver r(scriptScope);
        r.recurse(body);

//        ConstFolder cf;
//        cf.recurse(body);

        StringReplacer sr;
        sr.recurse(body);

        qil::Script s;
        s.variables = std::move(scriptScope.variables);
        s.strings = std::move(sr.strings);
        s.body = std::move(body);
        return s;
    }

    void analyze(ast::Statement::Ptr &node) {
        ScopeAdder sa;
        sa.recurse(node);

        Resolver r(interactiveScope);
        r.recurse(node);

//        ConstFolder cf;
//        cf.recurse(body);

        isr.recurse(node);
    }

    ~Analyzer() {
        interactiveScope.closeInteractive();
    }

private:
    StringReplacer isr{true};
    InteractiveScriptScope interactiveScriptScope;
    BlockScope interactiveScope{interactiveScriptScope};
};

} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_SCRIPT_ANALYZER_H_
