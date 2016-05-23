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
/// \brief Visitor for the first pass of expression analysis.
///
//------------------------------------------------------------------------------
#ifndef LIB_COMP_SEM_EXPRESSIONANALYZERPASS1_H_
#define LIB_COMP_SEM_EXPRESSIONANALYZERPASS1_H_

#include <string>
#include <vector>
#include "qore/core/util/Debug.h"
#include "qore/comp/ast/Expression.h"
#include "qore/comp/ast/Closure.h"
#include "qore/comp/sem/expr/Expression.h"
#include "qore/comp/sem/expr/AssignmentExpression.h"
#include "qore/comp/sem/expr/CompoundAssignmentExpression.h"
#include "qore/comp/sem/expr/FunctionCallExpression.h"
#include "qore/comp/sem/expr/FunctionGroupExpression.h"
#include "qore/comp/sem/expr/GlobalVariableRefExpression.h"
#include "qore/comp/sem/expr/IntLiteralExpression.h"
#include "qore/comp/sem/expr/InvokeBinaryOperatorExpression.h"
#include "qore/comp/sem/expr/InvokeConversionExpression.h"
#include "qore/comp/sem/expr/LocalVariableInitExpression.h"
#include "qore/comp/sem/expr/LocalVariableRefExpression.h"
#include "qore/comp/sem/expr/NothingLiteralExpression.h"
#include "qore/comp/sem/expr/StringLiteralRefExpression.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Resolves the type of an expression and translates its AST representation to a temporary representation.
 *
 * Resolves names and operators, deals with local variable declarations and adds implicit conversions.
 *
 * Implements the visitor for ast::Expression.
 */
class ExpressionAnalyzerPass1 {

public:
    /**
     * \brief Analyzes an expression.
     * \param core the shared state of the analyzer
     * \param scope the current scope
     * \param node the AST node to analyze
     * \return analyzed node
     */
    static Expression::Ptr eval(Core &core, Scope &scope, ast::Expression &node) {
        ExpressionAnalyzerPass1 a(core, scope);
        return node.accept(a);
    }

    /**
     * \brief Analyzes an expression ensuring certain type of the expression's value.
     * \param core the shared state of the analyzer
     * \param scope the current scope
     * \param type the desired type of the expression's value
     * \param node the AST node to analyze
     * \return analyzed node
     */
    static Expression::Ptr evalAndConvert(Core &core, Scope &scope, const Type &type, ast::Expression &node) {
        ExpressionAnalyzerPass1 a(core, scope, &type);
        return a.convert(node.accept(a), type);
    }

    ///\name Implementation of ast::Expression visitor
    ///\{
    using ReturnType = Expression::Ptr;

    Expression::Ptr visit(const ast::VarDeclExpression &node) {
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
        return LocalVariableInitExpression::create(lv, std::move(rhs));
    }

    Expression::Ptr visit(const ast::BinaryExpression &node) {
        Expression::Ptr left = eval(*node.left);
        Expression::Ptr right = eval(*node.right);
        const BinaryOperator &op = resolveOperator(BinaryOperator::Kind::Plus, left->getType(), right->getType());
        return InvokeBinaryOperatorExpression::create(
                op,
                convert(std::move(left), op.getLeftType()),
                convert(std::move(right), op.getRightType()));
    }

    Expression::Ptr visit(const ast::NameExpression &node) {
        Symbol s = scope.resolveSymbol(node.name);
        switch (s.getKind()) {
            case Symbol::Kind::FunctionGroup:
                return FunctionGroupExpression::create(s.asFunctionGroup());
            case Symbol::Kind::Global:
                return GlobalVariableRefExpression::create(s.asGlobal());
            case Symbol::Kind::Local:
                return LocalVariableRefExpression::create(s.asLocal());
            default:
                QORE_NOT_IMPLEMENTED("");
        }
    }

    Expression::Ptr visit(const ast::LiteralExpression &node) {
        if (node.token.type == qore::comp::TokenType::String) {
            //FIXME proper string literal parsing - no quotes, escapes etc.
            std::string s = core.getContext().getLexeme(node.token);
            s = s.substr(1, s.length() - 2);
            return StringLiteralRefExpression::create(core.createStringLiteral(s));
        } else if (node.token.type == qore::comp::TokenType::Integer) {
            //FIXME proper integer literal parsing
            std::string s = core.getContext().getLexeme(node.token);
            std::stringstream str(s);
            qint v;
            str >> v;
            return IntLiteralExpression::create(v);
        } else {
            QORE_NOT_IMPLEMENTED("");
        }
    }

    Expression::Ptr visit(const ast::AssignmentExpression &node) {
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

    Expression::Ptr visit(const ast::CallExpression &node) {
        std::vector<Expression::Ptr> args;
        std::vector<const Type *> argTypes;
        for (auto &arg : node.argList->data) {
            Expression::Ptr e = eval(*arg);
            argTypes.push_back(&e->getType());
            args.push_back(std::move(e));
        }
        Expression::Ptr callee = eval(*node.callee);
        if (callee->getKind() == Expression::Kind::FunctionGroup) {
            //FIXME catch exception and report diagnostic
            OverloadResolutionResult overload = static_cast<FunctionGroupExpression &>(*callee).getFunctionGroup()
                    .getRt().resolveOverload(argTypes);
            std::vector<const Conversion *> conversions = overload.getArgConversions();
            assert(args.size() == conversions.size());
            for (Index i = 0; i < args.size(); ++i) {
                if (conversions[i]) {
                    args[i] = InvokeConversionExpression::create(*conversions[i], std::move(args[i]));
                }
            }
            return FunctionCallExpression::create(overload.getFunction(), std::move(args));
        } else {
            //the type of callee should be either a reference to code, closure, or any
            QORE_NOT_IMPLEMENTED("");
        }
    }

    Expression::Ptr visit(const ast::ErrorExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::ListExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::HashExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::CastExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::UnaryExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::IndexExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::AccessExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::NewExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::InstanceofExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::ConditionalExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::ListOperationExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::RegexExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    Expression::Ptr visit(const ast::ClosureExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    ///\}

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
    const Type *typeHint;       //XXX this can be used for type inference, e.g. hash<float, softstring> = {1: 4}
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_EXPRESSIONANALYZERPASS1_H_
