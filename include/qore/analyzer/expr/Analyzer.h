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
#ifndef INCLUDE_QORE_ANALYZER_EXPR_ANALYZER_H_
#define INCLUDE_QORE_ANALYZER_EXPR_ANALYZER_H_

#include "qore/analyzer/Scope.h"
#include "qore/ast/Expression.h"
#include "qore/common/Util.h"       //TODO remove

#include "qore/qil/Code.h"

namespace qore {
namespace analyzer {
namespace expr {

#define INS(op, ...) code.add(qil::Instruction(qil::Opcode::op, ## __VA_ARGS__))

#define EVAL_LVALUE(node) { LValueEvaluator visitor(scope, code);  node->accept(visitor); }
#define EVAL_VALUE(node) { ValueEvaluator<true> visitor(scope, code);  node->accept(visitor); }
#define EVAL(node) { ValueEvaluator<false> visitor(scope, code);  node->accept(visitor); }

class LValueEvaluator : public ast::ExpressionVisitor {

public:
    LValueEvaluator(Scope &scope, qil::Code &code) : scope(scope), code(code) {
    }

    void visit(const ast::IntegerLiteral *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::StringLiteral *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::BinaryExpression *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::UnaryExpression *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::Assignment *node) override {
        errLValueExpected(node);
    }

    void visit(const ast::VarDecl *node) override {
        INS(LoadLocVarPtr, scope.createLocalVariable(node->name, node->getRange()));
    }

    void visit(const ast::Identifier *node) override {
        INS(LoadLocVarPtr, scope.resolve(node->name, node->getRange()));
    }

private:
    void errLValueExpected(const ast::Node *node) {
        //TODO report error
        QORE_UNREACHABLE("Not implemented");
    }

private:
    Scope &scope;
    qil::Code &code;
};

template<bool needsValue>
class ValueEvaluator : public ast::ExpressionVisitor {

public:
    ValueEvaluator(Scope &scope, qil::Code &code) : scope(scope), code(code) {
    }

    void visit(const ast::IntegerLiteral *node) override {
        QORE_UNREACHABLE("Push int");
        checkNoEffect();
    }

    void visit(const ast::StringLiteral *node) override {
        INS(PushString, scope.createStringLiteral(node->value, node->getRange()));
        checkNoEffect();
    }

    void visit(const ast::BinaryExpression *node) override {
        EVAL_VALUE(node->left);
        EVAL_VALUE(node->right);
        INS(Add);
        checkNoEffect();
    }

    void visit(const ast::UnaryExpression *node) override {
        EVAL_LVALUE(node->operand);
        INS(LoadUnique);
        INS(Trim);
        dup();
        INS(Swap);
        INS(CleanupLValue);
        INS(PopAndDeref);
    }

    void visit(const ast::Assignment *node) override {
        EVAL_VALUE(node->right);

        //this can be done only if the type of lhs is known at compile time:
        //type conversions
        dup();

        EVAL_LVALUE(node->left);

        //otherwise it will need to be done here

        INS(Swap);
        INS(CleanupLValue);
        INS(PopAndDeref);
    }

    void visit(const ast::VarDecl *node) override {
        scope.createLocalVariable(node->name, node->getRange());
        if (needsValue) {
            INS(PushNothing);
        }
    }

    void visit(const ast::Identifier *node) override {
        INS(PushLocVar, scope.resolve(node->name, node->getRange()));
        checkNoEffect();
    }

private:
    void dup() {
        if (needsValue) {
            INS(Dup);
        }
    }
    void checkNoEffect() {
        if (!needsValue) {
            //error statement has no effect
            //produce code that derefs result
        }
    }

private:
    Scope &scope;
    qil::Code &code;
};

class Analyzer {

public:
    Analyzer() = default;

    void eval(Scope &scope, qil::Code &code, const ast::Expression::Ptr &node) {
        EVAL(node);
    }

    void evalValue(Scope &scope, qil::Code &code, const ast::Expression::Ptr &node) {
        EVAL_VALUE(node);
    }

private:
    Analyzer(const Analyzer &) = delete;
    Analyzer(Analyzer &&) = delete;
    Analyzer &operator=(const Analyzer &) = delete;
    Analyzer &operator=(Analyzer &&) = delete;
};

#undef INS
#undef EVAL_LVALUE
#undef EVAL_VALUE
#undef EVAL

} // namespace expr
} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_EXPR_ANALYZER_H_
