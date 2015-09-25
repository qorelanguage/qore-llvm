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
#ifndef TEST_AST_ASTTESTHELPER_H_
#define TEST_AST_ASTTESTHELPER_H_

#include <memory>
#include "gmock/gmock.h"
#include "qore/ast/Expression.h"
#include "qore/ast/Statement.h"
#include "qore/ast/Visitor.h"

template<typename D, typename B>
typename D::Ptr ast_cast(std::unique_ptr<B> &&bPtr, const char *file, int line, const char *valueS, const char *typeS) {
    D *d = dynamic_cast<D *>(bPtr.get());
    if (d == nullptr) {
        ADD_FAILURE_AT(file, line) << valueS << " was expected to return "
                << typeS << ", but returned " << typeid(*bPtr.get()).name() << " instead";
    } else {
        bPtr.release();
    }
    return typename D::Ptr(d);
}

template<typename D, typename B>
D *ast_cast(const std::unique_ptr<B> &bPtr, const char *file, int line, const char *valueS, const char *typeS) {
    D *d = dynamic_cast<D *>(bPtr.get());
    if (d == nullptr) {
        ADD_FAILURE_AT(file, line) << valueS << " was expected to return "
                << typeS << ", but returned " << typeid(*bPtr.get()).name() << " instead";
    }
    return d;
}

#define AST_CAST(Type, Name, Value) \
    auto Name = ast_cast<Type>(Value, __FILE__, __LINE__, #Value, #Type); \
    if (!Name) return;

namespace qore {
namespace ast {

template<typename Base, typename VisitorType>
class NodeAdapter : public Base {

public:
    NodeAdapter(Base &node) : node(node) {
    }

    void accept(VisitorType &visitor) const override {
        node.accept(visitor);
    }

    SourceRange getRange() const override {
        return node.getRange();
    }

private:
    Base &node;
};

class MockExpression : public Expression {
public:
    MOCK_CONST_METHOD1(accept, void(ExpressionVisitor &));
    MOCK_CONST_METHOD0(getRange, SourceRange());

    operator Ptr() {
        return Ptr(new NodeAdapter<Expression, ExpressionVisitor>(*this));
    }
};

class MockStatement : public Statement {
public:
    MOCK_CONST_METHOD1(accept, void(StatementVisitor &));
    MOCK_CONST_METHOD0(getRange, SourceRange());

    operator Ptr() {
        return Ptr(new NodeAdapter<Statement, StatementVisitor>(*this));
    }
};

class MockExpressionVisitor : public ExpressionVisitor {
public:
    MOCK_METHOD1(visit, void(const IntegerLiteral *));
    MOCK_METHOD1(visit, void(const StringLiteral *));
    MOCK_METHOD1(visit, void(const BinaryExpression *));
    MOCK_METHOD1(visit, void(const UnaryExpression *));
    MOCK_METHOD1(visit, void(const Assignment *));
    MOCK_METHOD1(visit, void(const VarDecl *));
    MOCK_METHOD1(visit, void(const Identifier *));
};

class MockStatementVisitor : public StatementVisitor {
public:
    MOCK_METHOD1(visit, void(const EmptyStatement *));
    MOCK_METHOD1(visit, void(const PrintStatement *));
    MOCK_METHOD1(visit, void(const ExpressionStatement *));
    MOCK_METHOD1(visit, void(const CompoundStatement *));
    MOCK_METHOD1(visit, void(const IfStatement *));
    MOCK_METHOD1(visit, void(const TryStatement *));
};

class MockProgramVisitor : public ProgramVisitor {
public:
    MOCK_METHOD1(visit, void(const Program *));
};

} // namespace ast
} // namespace qore

#endif // TEST_AST_ASTTESTHELPER_H_
