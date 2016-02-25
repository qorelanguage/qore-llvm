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
/// \brief Base classes for implementing AST node visitors.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_VISITORBASE_H_
#define INCLUDE_QORE_COMP_AST_VISITORBASE_H_

#include "qore/comp/ast/Visitor.h"
#include "qore/comp/ast/Script.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Base class for expression visitors.
 */
class ExpressionVisitorBase : public ExpressionVisitor {

public:
    void visit(ErrorExpression &node) override {}
    void visit(LiteralExpression &node) override {}
    void visit(NameExpression &node) override {}
    void visit(ListExpression &node) override {}
    void visit(HashExpression &node) override {}
    void visit(VarDeclExpression &node) override {}
    void visit(CastExpression &node) override {}
    void visit(CallExpression &node) override {}
    void visit(UnaryExpression &node) override {}
    void visit(IndexExpression &node) override {}
    void visit(AccessExpression &node) override {}
    void visit(NewExpression &node) override {}
    void visit(BinaryExpression &node) override {}
    void visit(InstanceofExpression &node) override {}
    void visit(ConditionalExpression &node) override {}
    void visit(AssignmentExpression &node) override {}
    void visit(ListOperationExpression &node) override {}
    void visit(RegexExpression &node) override {}
    void visit(ClosureExpression &node) override {}
};

/**
 * \brief Base class for statement visitors.
 */
class StatementVisitorBase : public StatementVisitor {

public:
    void visit(EmptyStatement &node) override {}
    void visit(ExpressionStatement &node) override {}
    void visit(CompoundStatement &node) override {}
    void visit(ReturnStatement &node) override {}
    void visit(IfStatement &node) override {}
    void visit(TryStatement &node) override {}
    void visit(ForeachStatement &node) override {}
    void visit(ThrowStatement &node) override {}
    void visit(SimpleStatement &node) override {}
    void visit(ScopeGuardStatement &node) override {}
    void visit(WhileStatement &node) override {}
    void visit(DoWhileStatement &node) override {}
    void visit(ForStatement &node) override {}
    void visit(SwitchStatement &node) override {}
};

/**
 * \brief Base class for visitors of declarations (namespaces, classes, globals, constants, functions).
 */
class DeclarationVisitorBase : public DeclarationVisitor {

public:
    void visit(Script &node) override {}
    void visit(Namespace &node) override {}
    void visit(Class &node) override {}
    void visit(NamespaceConstant &node) override {}
    void visit(GlobalVariable &node) override {}
    void visit(Function &node) override {}
    void visit(Method &node) override {}
    void visit(ClassConstant &node) override {}
    void visit(MemberGroup &node) override {}
    void visit(Field &node) override {}
};

/**
 * \brief Base class for visitors of types.
 */
class TypeVisitorBase : public TypeVisitor {

public:
    void visit(NameType &node) override {}
    void visit(AsteriskType &node) override {}
    void visit(ImplicitType &node) override {}
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_VISITORBASE_H_
