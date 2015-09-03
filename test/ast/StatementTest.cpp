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
#include "Mocks.h"
#include "qore/ast/Statement.h"

namespace qore {
namespace ast {

struct StatementTest : ::testing::Test {
    void *retVal{this};
    ::testing::StrictMock<MockVisitor> mockVisitor;
};

TEST_F(StatementTest, EmptyStatement) {
    EmptyStatement::Ptr node = EmptyStatement::create();
    EXPECT_CALL(mockVisitor, visit(node.get())).WillOnce(::testing::Return(retVal));
    EXPECT_EQ(retVal, node->accept(mockVisitor));
}

TEST_F(StatementTest, PrintStatement) {
    PrintStatement::Ptr node = PrintStatement::create(MockExpression::create());
    EXPECT_CALL(mockVisitor, visit(node.get())).WillOnce(::testing::Return(retVal));
    EXPECT_EQ(retVal, node->accept(mockVisitor));
}

} // namespace ast
} // namespace qore
