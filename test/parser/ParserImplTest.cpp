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

    void EXPECT_CONSUMED() { EXPECT_FALSE(parser.hasToken); }
    void EXPECT_NOT_CONSUMED() { EXPECT_TRUE(parser.hasToken); }
};

TEST_F(ParserImplTest, parse) {
    addToken(TokenType::Semicolon);
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    ast::Program::Ptr prg = parser.parse();
    EXPECT_EQ(1U, prg->body.size());
    AST_CAST(ast::EmptyStatement, stmt, prg->body[0]);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, parseStatementNone) {
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    EXPECT_EQ(nullptr, parser.parseStatement().get());
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, parseStatementSingle) {
    addToken(TokenType::Semicolon);
    DIAG_NONE();
    AST_CAST(ast::EmptyStatement, stmt, parser.parseStatement());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, program) {
    addToken(TokenType::Semicolon);
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    ast::Program::Ptr prg = program();
    EXPECT_EQ(1U, prg->body.size());
    AST_CAST(ast::EmptyStatement, stmt, prg->body[0]);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, statementsNone) {
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    ast::Statements stmts = statements();
    EXPECT_EQ(0U, stmts.size());
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, statementsSingle) {
    addToken(TokenType::Semicolon);
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    ast::Statements stmts = statements();
    EXPECT_EQ(1U, stmts.size());
    AST_CAST(ast::EmptyStatement, stmt, stmts[0]);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, statementsDouble) {
    addToken(TokenType::Semicolon);
    addToken(TokenType::KwPrint);
    addToken(TokenType::Integer, 123);
    addToken(TokenType::Semicolon);
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    ast::Statements stmts = statements();
    EXPECT_EQ(2U, stmts.size());
    AST_CAST(ast::EmptyStatement, stmt0, stmts[0]);
    AST_CAST(ast::PrintStatement, stmt1, stmts[1]);
    AST_CAST(ast::IntegerLiteral, expr, stmt1->expression);
    EXPECT_EQ(123U, expr->value);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, statementEmpty) {
    addToken(TokenType::Semicolon);
    DIAG_NONE();
    AST_CAST(ast::EmptyStatement, stmt, statement());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, statementPrint) {
    addToken(TokenType::KwPrint);
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Semicolon);
    DIAG_NONE();
    AST_CAST(ast::PrintStatement, stmt, statement());
    AST_CAST(ast::IntegerLiteral, expr, stmt->expression);
    EXPECT_EQ(1234U, expr->value);
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, statementErr) {
    addToken(TokenType::String, "abc", SourceTestHelper::createLocation(44, 55));
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Semicolon);
    DIAG_EXPECT(DiagId::ParserStatementExpected, 44, 55);
    AST_CAST(ast::EmptyStatement, stmt, statement());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, statementRecoverEof) {
    addToken(TokenType::KwPrint);
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Integer, 1235, SourceTestHelper::createLocation(33, 44));
    addToken(TokenType::Integer, 1236);
    addToken(TokenType::Integer, 1237);
    addToken(TokenType::EndOfFile);
    DIAG_EXPECT(DiagId::ParserUnexpectedToken, 33, 44);
    AST_CAST(ast::PrintStatement, stmt, statement());
    AST_CAST(ast::IntegerLiteral, expr, stmt->expression);
    EXPECT_EQ(1234U, expr->value);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, printStatement) {
    addToken(TokenType::KwPrint);
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Semicolon);
    DIAG_NONE();
    ast::PrintStatement::Ptr stmt = printStatement();
    AST_CAST(ast::IntegerLiteral, expr, stmt->expression);
    EXPECT_EQ(1234U, expr->value);
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, printStatementErr) {
    addToken(TokenType::KwPrint);
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Integer, 1235, SourceTestHelper::createLocation(22, 33));
    addToken(TokenType::Integer, 1236);
    addToken(TokenType::Integer, 1237);
    addToken(TokenType::Semicolon);
    DIAG_EXPECT(DiagId::ParserUnexpectedToken, 22, 33);
    ast::PrintStatement::Ptr stmt = printStatement();
    AST_CAST(ast::IntegerLiteral, expr, stmt->expression);
    EXPECT_EQ(1234U, expr->value);
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, expression) {
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Plus);
    addToken(TokenType::String, "abc");
    addToken(TokenType::Integer, 1111);
    DIAG_NONE();
    AST_CAST(ast::BinaryExpression, expr, expression());
    AST_CAST(ast::IntegerLiteral, left, expr->left);
    AST_CAST(ast::StringLiteral, right, expr->right);
    EXPECT_EQ(1234U, left->value);
    EXPECT_EQ("abc", right->value);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, additiveSingle) {
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Integer, 1111);
    DIAG_NONE();
    AST_CAST(ast::IntegerLiteral, expr, additiveExpression());
    EXPECT_EQ(1234U, expr->value);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, additiveDouble) {
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Plus);
    addToken(TokenType::String, "abc");
    addToken(TokenType::Integer, 1111);
    DIAG_NONE();
    AST_CAST(ast::BinaryExpression, expr, additiveExpression());
    AST_CAST(ast::IntegerLiteral, left, expr->left);
    AST_CAST(ast::StringLiteral, right, expr->right);
    EXPECT_EQ(1234U, left->value);
    EXPECT_EQ("abc", right->value);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, additiveTriple) {
    addToken(TokenType::Integer, 1234);
    addToken(TokenType::Plus);
    addToken(TokenType::String, "abc");
    addToken(TokenType::Plus);
    addToken(TokenType::Integer, 1111);
    addToken(TokenType::Semicolon);
    DIAG_NONE();
    AST_CAST(ast::BinaryExpression, expr, additiveExpression());
    AST_CAST(ast::BinaryExpression, left, expr->left);
    AST_CAST(ast::IntegerLiteral, right, expr->right);
    AST_CAST(ast::IntegerLiteral, leftLeft, left->left);
    AST_CAST(ast::StringLiteral, leftRight, left->right);
    EXPECT_EQ(1234U, leftLeft->value);
    EXPECT_EQ("abc", leftRight->value);
    EXPECT_EQ(1111U, right->value);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, primaryInteger) {
    addToken(TokenType::Integer, 1234);
    DIAG_NONE();
    AST_CAST(ast::IntegerLiteral, expr, primaryExpression());
    EXPECT_EQ(1234U, expr->value);
}

TEST_F(ParserImplTest, primaryString) {
    addToken(TokenType::String, "abc");
    DIAG_NONE();
    AST_CAST(ast::StringLiteral, expr, primaryExpression());
    EXPECT_EQ("abc", expr->value);
}

TEST_F(ParserImplTest, primaryFail) {
    addToken(TokenType::KwPrint, SourceTestHelper::createLocation(11, 22));
    DIAG_EXPECT(DiagId::ParserExpectedPrimaryExpression, 11, 22);
    AST_CAST(ast::IntegerLiteral, expr, primaryExpression());
    EXPECT_EQ(0U, expr->value);
    EXPECT_CONSUMED();
}

} // namespace qore
