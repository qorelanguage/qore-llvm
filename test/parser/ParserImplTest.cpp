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
#include "gtest/gtest.h"
#include "qore/parser/ParserImpl.h"
#include "../ast/AstTestHelper.h"
#include "../context/DiagTestHelper.h"
#include "../scanner/ScannerTestHelper.h"

namespace qore {

struct ParserImplTest : ::testing::Test, DiagTestHelper, ScannerTestHelper {
    ParserImpl parser{diagMgr, scanner};

    ast::Program::Ptr program() { return parser.program(); }
    ast::Statements statements() { return parser.statements(); }
    ast::Statement::Ptr statement() { return parser.statement(); }
    ast::PrintStatement::Ptr printStatement() { return parser.printStatement(); }
    ast::Expression::Ptr expression() { return parser.expression(); }
    ast::Expression::Ptr additiveExpression() { return parser.additiveExpression(); }
    ast::Expression::Ptr primaryExpression() { return parser.primaryExpression(); }
};

TEST_F(ParserImplTest, PrimaryInteger) {
    addToken(TokenType::Integer, 1234);
    AST_CAST(ast::IntegerLiteral, expr, primaryExpression());
    EXPECT_EQ(1234U, expr->value);
}

} // namespace qore
