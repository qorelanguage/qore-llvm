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
#include "qore/ast/Expression.h"
#include "../context/SourceTestHelper.h"
#include "AstTestHelper.h"

namespace qore {
namespace ast {

struct ExpressionTest : ::testing::Test {
    ::testing::StrictMock<MockExpressionVisitor> mockVisitor;
    SourceRange range = SourceTestHelper::createRange(11, 22, 33);
};

TEST_F(ExpressionTest, IntegerLiteral) {
    IntegerLiteral::Ptr node = IntegerLiteral::create(range, 1234);
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ(1234U, node->value);
    EXPECT_EQ(range, node->getRange());
}

TEST_F(ExpressionTest, StringLiteral) {
    StringLiteral::Ptr node = StringLiteral::create(range, "test");
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ("test", node->value);
    EXPECT_EQ(range, node->getRange());
}

TEST_F(ExpressionTest, BinaryExpression) {
    MockExpression left;
    MockExpression right;

    BinaryExpression::Ptr node = BinaryExpression::create(left, range, right);
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ(range, node->operatorRange);

    EXPECT_CALL(left, getRange()).WillOnce(::testing::Return(SourceTestHelper::createRange(1, 2, 3)));
    EXPECT_CALL(right, getRange()).WillOnce(::testing::Return(SourceTestHelper::createRange(4, 5, 6)));
    SourceRange r = node->getRange();
    EXPECT_EQ(1, r.start.line);
    EXPECT_EQ(2, r.start.column);
    EXPECT_EQ(4, r.end.line);
    EXPECT_EQ(6, r.end.column);
}

TEST_F(ExpressionTest, UnaryExpression) {
    MockExpression operand;

    UnaryExpression::Ptr node = UnaryExpression::create(range, operand);
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ(range, node->operatorRange);

    EXPECT_CALL(operand, getRange()).WillOnce(::testing::Return(SourceTestHelper::createRange(1, 2, 3)));
    SourceRange r = node->getRange();
    EXPECT_EQ(11, r.start.line);
    EXPECT_EQ(22, r.start.column);
    EXPECT_EQ(1, r.end.line);
    EXPECT_EQ(3, r.end.column);
}

TEST_F(ExpressionTest, Assignment) {
    MockExpression left;
    MockExpression right;

    Assignment::Ptr node = Assignment::create(left, range, right);
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ(range, node->operatorRange);

    EXPECT_CALL(left, getRange()).WillOnce(::testing::Return(SourceTestHelper::createRange(1, 2, 3)));
    EXPECT_CALL(right, getRange()).WillOnce(::testing::Return(SourceTestHelper::createRange(4, 5, 6)));
    SourceRange r = node->getRange();
    EXPECT_EQ(1, r.start.line);
    EXPECT_EQ(2, r.start.column);
    EXPECT_EQ(4, r.end.line);
    EXPECT_EQ(6, r.end.column);
}

TEST_F(ExpressionTest, VarDecl) {
    VarDecl::Ptr node = VarDecl::create(range, "varName");
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ("varName", node->name);
    EXPECT_EQ(range, node->getRange());
}

TEST_F(ExpressionTest, Identifier) {
    Identifier::Ptr node = Identifier::create(range, "varName");
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ("varName", node->name);
    EXPECT_EQ(range, node->getRange());
}

} // namespace ast
} // namespace qore
