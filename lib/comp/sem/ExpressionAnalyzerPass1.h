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
#include "qore/common/Exceptions.h"
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
    ExpressionAnalyzerPass1(Core &core, BlockScope &scope, const as::Type *typeHint = nullptr)
            : core(core), scope(scope), typeHint(typeHint) {
    }

    Expression::Ptr visit(ast::VarDeclExpression &node) override {
        const as::Type &type = scope.resolveType(node.type);
        LocalVariable &lv = scope.declareLocalVariable(node.name, node.location, type);

        Expression::Ptr rhs;
        if (node.initializer) {
            //XXX check that the new variable is not used before initialization, but it needs to be in scope:
            //                  code f = sub() {    f();    }
            rhs = evalAndConvert(*node.initializer, type);
        } else {
            rhs = defaultFor(core, type);
        }
        return LifetimeStartExpression::create(lv, std::move(rhs));
    }

    Expression::Ptr visit(ast::BinaryExpression &node) override {
        Expression::Ptr left = eval(*node.left);
        Expression::Ptr right = eval(*node.right);
        const rt::meta::BinaryOperatorDesc &desc = resolveOperator(rt::Op::Plus, left->getType(), right->getType());
        return InvokeBinaryOperatorExpression::create(
                desc,
                convert(std::move(left), desc.leftType),
                convert(std::move(right), desc.rightType));
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
            std::string s = core.ctx.getSrcMgr().get(node.token.location.sourceId).getRange(
                    node.token.location.offset, node.token.length);
            s = s.substr(1, s.length() - 2);
            return StringLiteralRefExpression::create(core.createStringLiteral(s));
        } else if (node.token.type == qore::comp::TokenType::Integer) {
            //FIXME proper integer literal parsing
            std::string s = core.ctx.getSrcMgr().get(node.token.location.sourceId).getRange(
                    node.token.location.offset, node.token.length);
            std::stringstream str(s);
            rt::qint v;
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
            const rt::meta::BinaryOperatorDesc &desc
                    = resolveOperator(rt::Op::PlusEq, left->getType(), right->getType());
            return CompoundAssignmentExpression::create(
                    std::move(left),
                    desc,
                    convert(std::move(right), desc.rightType));
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

    static Expression::Ptr defaultFor(Core &core, const as::Type &type) {
        if (type == as::Type::String) {
            return StringLiteralRefExpression::create(core.createStringLiteral(""));
        }
        if (type == as::Type::Int) {
            return IntLiteralExpression::create(0);
        }
        if (type == as::Type::Any || type == as::Type::Error || type.isOptional()) {
            return NothingLiteralExpression::create();
        }
        QORE_NOT_IMPLEMENTED("Default value");
    }

private:
    Expression::Ptr eval(ast::Expression &node) {
        ExpressionAnalyzerPass1 a(core, scope);
        return node.accept(a);
    }

    Expression::Ptr evalAndConvert(ast::Expression &node, const as::Type &type) {
        ExpressionAnalyzerPass1 a(core, scope, &type);
        return convert(node.accept(a), type);
    }

    Expression::Ptr convert(Expression::Ptr expr, rt::Type type) {
        //FIXME catch exception and report diagnostic
        const rt::meta::ConversionDesc &desc = rt::meta::findConversion(expr->getType().getRuntimeType(), type);
        if (desc.conversion == rt::Conversion::Identity) {
            return std::move(expr);
        }
        return InvokeConversionExpression::create(desc, std::move(expr));
    }

    Expression::Ptr convert(Expression::Ptr expr, const as::Type &type) {
        return convert(std::move(expr), type.getRuntimeType());
    }

    const rt::meta::BinaryOperatorDesc &resolveOperator(rt::Op o, const as::Type &l, const as::Type &r) {
        //FIXME catch exception and report diagnostic
        return rt::meta::findOperator(o, l.getRuntimeType(), r.getRuntimeType());
    }

private:
    Core &core;
    BlockScope &scope;
    const as::Type *typeHint;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_EXPRESSIONANALYZERPASS1_H_
