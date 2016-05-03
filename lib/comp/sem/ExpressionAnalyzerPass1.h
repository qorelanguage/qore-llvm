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
#ifndef LIB_COMP_SEM_EXPRESSIONANALYZERPASS1_H_
#define LIB_COMP_SEM_EXPRESSIONANALYZERPASS1_H_

#include <string>
#include "qore/core/util/Debug.h"
#include "qore/comp/ast/Expression.h"
#include "qore/comp/ast/Closure.h"
#include "qore/comp/sem/expr/Expression.h"
#include "qore/comp/sem/expr/AssignmentExpression.h"
#include "qore/comp/sem/expr/CompoundAssignmentExpression.h"
#include "qore/comp/sem/expr/GlobalVariableRefExpression.h"
#include "qore/comp/sem/expr/IntLiteralExpression.h"
#include "qore/comp/sem/expr/InvokeBinaryOperatorExpression.h"
#include "qore/comp/sem/expr/InvokeConversionExpression.h"
#include "qore/comp/sem/expr/LifetimeStartExpression.h"
#include "qore/comp/sem/expr/LocalVariableRefExpression.h"
#include "qore/comp/sem/expr/NothingLiteralExpression.h"
#include "qore/comp/sem/expr/StringLiteralRefExpression.h"

namespace qore {
namespace comp {
namespace sem {

class ExpressionAnalyzerPass1 : public ast::ExpressionVisitor<Expression::Ptr> {

public:
    static Expression::Ptr eval(Core &core, Scope &scope, ast::Expression &node) {
        ExpressionAnalyzerPass1 a(core, scope);
        return node.accept(a);
    }

    static Expression::Ptr evalAndConvert(Core &core, Scope &scope, const Type &type, ast::Expression &node) {
        ExpressionAnalyzerPass1 a(core, scope, &type);
        return a.convert(node.accept(a), type);
    }

    Expression::Ptr visit(ast::VarDeclExpression &node) override {
        const Type &type = scope.resolveType(node.type);
        LocalVariableInfo &lv = scope.declareLocalVariable(node.name, node.location, type);

        Expression::Ptr rhs;
        if (node.initializer) {
            //XXX check that the new variable is not used before initialization, but it needs to be in scope:
            //                  code f = sub() {    f();    }
            rhs = evalAndConvert(*node.initializer, type);
        } else {
            rhs = core.defaultFor(type);
        }
        return LifetimeStartExpression::create(lv, std::move(rhs));
    }

    Expression::Ptr visit(ast::BinaryExpression &node) override {
        Expression::Ptr left = eval(*node.left);
        Expression::Ptr right = eval(*node.right);
        const BinaryOperator &op = resolveOperator(BinaryOperator::Kind::Plus, left->getType(), right->getType());
        return InvokeBinaryOperatorExpression::create(
                op,
                convert(std::move(left), op.getLeftType()),
                convert(std::move(right), op.getRightType()));
    }

    Expression::Ptr visit(ast::NameExpression &node) override {
        Symbol s = scope.resolveSymbol(node.name);
        if (s.getKind() == Symbol::Kind::Global) {
            return GlobalVariableRefExpression::create(s.asGlobal());
        } else if (s.getKind() == Symbol::Kind::Local) {
            return LocalVariableRefExpression::create(s.asLocal());
        } else {
            QORE_NOT_IMPLEMENTED("");
        }
    }

    Expression::Ptr visit(ast::LiteralExpression &node) override {
        if (node.token.type == qore::comp::TokenType::String) {
            //FIXME proper string literal parsing - no quotes, escapes etc.
            std::string s = core.ctx.getLexeme(node.token);
            s = s.substr(1, s.length() - 2);
            return StringLiteralRefExpression::create(core.scriptBuilder.createStringLiteral(s));
        } else if (node.token.type == qore::comp::TokenType::Integer) {
            //FIXME proper integer literal parsing
            std::string s = core.ctx.getLexeme(node.token);
            std::stringstream str(s);
            qint v;
            str >> v;
            return IntLiteralExpression::create(v);
        } else {
            QORE_NOT_IMPLEMENTED("");
        }
    }

    Expression::Ptr visit(ast::AssignmentExpression &node) override {
        if (node.op.type == qore::comp::TokenType::Equals) {
            Expression::Ptr left = eval(*node.left);
            Expression::Ptr right = evalAndConvert(*node.right, left->getType());
            return AssignmentExpression::create(std::move(left), std::move(right));
        } else if (node.op.type == qore::comp::TokenType::PlusEquals) {
            Expression::Ptr left = eval(*node.left);
            Expression::Ptr right = eval(*node.right);
            const BinaryOperator &op = resolveOperator(BinaryOperator::Kind::PlusEquals,
                    left->getType(), right->getType());
            return CompoundAssignmentExpression::create(
                    std::move(left),
                    op,
                    convert(std::move(right), op.getRightType()));
        } else {
            QORE_NOT_IMPLEMENTED("");
        }
    }

    Expression::Ptr visit(ast::ErrorExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::ListExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::HashExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::CastExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::CallExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::UnaryExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::IndexExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::AccessExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::NewExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::InstanceofExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::ConditionalExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::ListOperationExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::RegexExpression &node) override { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(ast::ClosureExpression &node) override { QORE_NOT_IMPLEMENTED(""); }

private:
    ExpressionAnalyzerPass1(Core &core, Scope &scope, const Type *typeHint = nullptr)
            : core(core), scope(scope), typeHint(typeHint) {
    }

    Expression::Ptr eval(ast::Expression &node) {
        ExpressionAnalyzerPass1 a(core, scope);
        return node.accept(a);
    }

    Expression::Ptr evalAndConvert(ast::Expression &node, const Type &type) {
        ExpressionAnalyzerPass1 a(core, scope, &type);
        return convert(node.accept(a), type);
    }

    Expression::Ptr convert(Expression::Ptr expr, const Type &type) {
        //FIXME catch exception and report diagnostic
        const Conversion *conversion = Conversion::find(expr->getType(), type);
        if (!conversion) {
            return std::move(expr);
        }
        return InvokeConversionExpression::create(*conversion, std::move(expr));
    }

    const BinaryOperator &resolveOperator(BinaryOperator::Kind kind, const Type &l, const Type &r) {
        //FIXME catch exception and report diagnostic
        return BinaryOperator::find(kind, l, r);
    }

private:
    Core &core;
    Scope &scope;
    const Type *typeHint;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_EXPRESSIONANALYZERPASS1_H_
