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
#include "qore/ast/Statement.h"
#include "../context/SourceTestHelper.h"
#include "AstTestHelper.h"

namespace qore {
namespace ast {

struct StatementTest : ::testing::Test {
    ::testing::StrictMock<MockStatementVisitor> mockVisitor;
    SourceRange range = SourceTestHelper::createRange(11, 22, 33);
};

TEST_F(StatementTest, EmptyStatement) {
    EmptyStatement::Ptr node = EmptyStatement::create(range);
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ(range, node->getRange());
}

TEST_F(StatementTest, PrintStatement) {
    MockExpression expr;
    PrintStatement::Ptr node = PrintStatement::create(range, expr);
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ(range, node->getRange());
}

TEST_F(StatementTest, ExpressionStatement) {
    MockExpression expr;
    ExpressionStatement::Ptr node = ExpressionStatement::create(range, expr);
    EXPECT_CALL(mockVisitor, visit(node.get())).Times(1);
    node->accept(mockVisitor);
    EXPECT_EQ(range, node->getRange());
}

} // namespace ast
} // namespace qore
