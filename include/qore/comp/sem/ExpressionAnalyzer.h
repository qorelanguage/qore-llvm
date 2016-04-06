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
#ifndef INCLUDE_QORE_COMP_SEM_EXPRESSIONANALYZER_H_
#define INCLUDE_QORE_COMP_SEM_EXPRESSIONANALYZER_H_

#include <string>
#include "qore/ir/expr/AssignmentExpression.h"
#include "qore/ir/expr/CompoundAssignmentExpression.h"
#include "qore/ir/expr/GlobalVariableRefExpression.h"
#include "qore/ir/expr/InvokeExpression.h"
#include "qore/ir/expr/LifetimeStartExpression.h"
#include "qore/ir/expr/LocalVariableRefExpression.h"
#include "qore/comp/sem/Scope.h"

namespace qore {
namespace comp {
namespace sem {

class ExpressionAnalyzer : public qore::comp::ast::ExpressionVisitor {

public:
    ExpressionAnalyzer(Context &ctx, BlockScope &scope) : ctx(ctx), scope(scope), typeHint(nullptr) {
    }

public:
    void visit(qore::comp::ast::ErrorExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::ListExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::HashExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::CastExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::CallExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::UnaryExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::IndexExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::AccessExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::NewExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::InstanceofExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::ConditionalExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::ListOperationExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::RegexExpression &node) { QORE_NOT_IMPLEMENTED(""); }
    void visit(qore::comp::ast::ClosureExpression &node) { QORE_NOT_IMPLEMENTED(""); }

    void visit(qore::comp::ast::BinaryExpression &node) {
        ir::Expression::Ptr left = eval(*node.left);
        ir::Expression::Ptr right = eval(*node.right);
        const ir::Function &f = scope.resolveOperatorAdd(left->getType(), right->getType());
        result = ir::InvokeExpression::create(
                f,
                convert(std::move(left), f.getArgType(0)),
                convert(std::move(right), f.getArgType(1)));
    }

    void visit(qore::comp::ast::NameExpression &node) {
        Symbol s = scope.resolve(node.name);
        if (s.getKind() == Symbol::Kind::Global) {
            result = ir::GlobalVariableRefExpression::create(s.asGlobal());
        } else if (s.getKind() == Symbol::Kind::Local) {
            result = ir::LocalVariableRefExpression::create(s.asLocal());
        } else {
            QORE_NOT_IMPLEMENTED("");
        }
    }

    void visit(qore::comp::ast::LiteralExpression &node) {
        if (node.token.type == qore::comp::TokenType::String) {
            //FIXME proper string literal parsing - no quotes, escapes etc.
            std::string s = ctx.getSrcMgr().get(node.token.location.sourceId).getRange(
                    node.token.location.offset, node.token.length);
            s = s.substr(1, s.length() - 2);
            result = ir::StringLiteralRefExpression::create(scope.createStringLiteral(s));
        } else if (node.token.type == qore::comp::TokenType::Integer) {
            //FIXME proper integer literal parsing
            std::string s = ctx.getSrcMgr().get(node.token.location.sourceId).getRange(
                    node.token.location.offset, node.token.length);
            std::stringstream str(s);
            rt::qint v;
            str >> v;
            result = ir::IntLiteralExpression::create(v);
        } else {
            QORE_NOT_IMPLEMENTED("");
        }
    }


    void visit(qore::comp::ast::AssignmentExpression &node) {
        if (node.op.type == qore::comp::TokenType::Equals) {
            ir::Expression::Ptr left = eval(*node.left);
            ir::Expression::Ptr right = eval(*node.right, &left->getType());
            result = ir::AssignmentExpression::create(std::move(left), std::move(right));
        } else if (node.op.type == qore::comp::TokenType::PlusEquals) {
            ir::Expression::Ptr left = eval(*node.left);
            ir::Expression::Ptr right = eval(*node.right);
            const ir::Function &f = scope.resolveOperatorAdd(left->getType(), right->getType());
            if (f.getRetType() != left->getType()) {
                QORE_NOT_IMPLEMENTED("");           //e.g. int i; float f; i += f;
            }
            result = ir::CompoundAssignmentExpression::create(
                    std::move(left),
                    f,
                    convert(std::move(right), f.getArgType(1)));
        } else {
            QORE_NOT_IMPLEMENTED("");
        }
    }

    void visit(qore::comp::ast::VarDeclExpression &node) {
        const ir::Type &type = scope.resolveType(node.type);
        Symbol v = scope.declareLocalVariable(node.name, type);

        ir::Expression::Ptr rhs;
        if (node.initializer) {
            //XXX check that the new variable is not used before initialization, but it needs to be in scope:
            //                  code f = sub() {    f();    }
            rhs = eval(*node.initializer, &type);
        } else {
            rhs = scope.defaultFor(type);
        }
        result = ir::LifetimeStartExpression::create(v.asLocal(), std::move(rhs));
    }

private:
    ir::Expression::Ptr eval(ast::Expression &node, const ir::Type *type = nullptr) {
        const ir::Type *o = typeHint;
        typeHint = type;
        node.accept(*this);
        typeHint = o;

        if (type && result->getType() != *type) {
            return convert(std::move(result), *type);
        }

        return std::move(result);
    }

    ir::Expression::Ptr convert(ir::Expression::Ptr expr, const ir::Type &type) {
        const ir::Function *f = scope.resolveConversion(expr->getType(), type);
        if (!f) {
            return std::move(expr);
        }
        return ir::InvokeExpression::create(*f, std::move(expr));
    }

private:
    Context &ctx;
    BlockScope &scope;
    const ir::Type *typeHint;

public:
    ir::Expression::Ptr result;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPRESSIONANALYZER_H_
