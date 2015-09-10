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
    ast::ExpressionStatement::Ptr expressionStatement() { return parser.expressionStatement(); }
    ast::Expression::Ptr expression() { return parser.expression(); }
    ast::Expression::Ptr additiveExpression() { return parser.additiveExpression(); }
    ast::Expression::Ptr prefixExpression() { return parser.prefixExpression(); }
    ast::Expression::Ptr primaryExpression() { return parser.primaryExpression(); }
    ast::VarDecl::Ptr varDecl() { return parser.varDecl(); }

    void EXPECT_CONSUMED() { EXPECT_FALSE(parser.hasToken); }
    void EXPECT_NOT_CONSUMED() { EXPECT_TRUE(parser.hasToken); }

    SourceRange range1 = createRange(11, 12, 13);
    SourceRange range2 = createRange(21, 22, 23);
    SourceRange range3 = createRange(31, 32, 33);
    SourceRange range4 = createRange(41, 42, 43);
    SourceRange range5 = createRange(51, 52, 53);
    SourceRange range6 = createRange(61, 62, 63);
    SourceRange range7 = createRange(71, 72, 73);
};

TEST_F(ParserImplTest, parse) {
    addToken(TokenType::Semicolon, range1);
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    ast::Program::Ptr prg = parser.parse();
    EXPECT_EQ(1U, prg->body.size());
    AST_CAST(ast::EmptyStatement, stmt, prg->body[0]);
    EXPECT_EQ(range1, prg->getRange());
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
    addToken(TokenType::Semicolon, range1);
    addToken(TokenType::Semicolon, range2);
    addToken(TokenType::EndOfFile);
    DIAG_NONE();
    ast::Program::Ptr prg = program();
    EXPECT_EQ(2U, prg->body.size());
    AST_CAST(ast::EmptyStatement, stmt, prg->body[0]);
    EXPECT_EQ(SourceRange(range1.start, range2.end), prg->getRange());
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

TEST_F(ParserImplTest, statementExpr) {
    addToken(TokenType::String, "abc", range1);
    addToken(TokenType::Semicolon, range3);
    DIAG_NONE();
    AST_CAST(ast::ExpressionStatement, stmt, statement());
    AST_CAST(ast::StringLiteral, expr, stmt->expression);
    EXPECT_EQ(SourceRange(range1.start, range3.end), stmt->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, statementRecoverEof) {
    addToken(TokenType::KwPrint, range1);
    addToken(TokenType::Integer, 1234, range2);
    addToken(TokenType::Integer, 1235, range3);
    addToken(TokenType::Integer, 1236, range4);
    addToken(TokenType::EndOfFile, range5);
    DIAG_EXPECT(DiagId::ParserUnexpectedToken, 31, 32);
    AST_CAST(ast::PrintStatement, stmt, statement());
    AST_CAST(ast::IntegerLiteral, expr, stmt->expression);
    EXPECT_EQ(1234U, expr->value);
    EXPECT_EQ(SourceRange(range1.start, range5.end), stmt->getRange());
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, printStatement) {
    addToken(TokenType::KwPrint, range1);
    addToken(TokenType::Integer, 1234, range2);
    addToken(TokenType::Semicolon, range3);
    DIAG_NONE();
    ast::PrintStatement::Ptr stmt = printStatement();
    AST_CAST(ast::IntegerLiteral, expr, stmt->expression);
    EXPECT_EQ(1234U, expr->value);
    EXPECT_EQ(SourceRange(range1.start, range3.end), stmt->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, printStatementErr) {
    addToken(TokenType::KwPrint, range1);
    addToken(TokenType::Integer, 1234, range2);
    addToken(TokenType::Integer, 1235, range3);
    addToken(TokenType::Integer, 1236, range4);
    addToken(TokenType::Semicolon, range5);
    DIAG_EXPECT(DiagId::ParserUnexpectedToken, 31, 32);
    ast::PrintStatement::Ptr stmt = printStatement();
    AST_CAST(ast::IntegerLiteral, expr, stmt->expression);
    EXPECT_EQ(1234U, expr->value);
    EXPECT_EQ(SourceRange(range1.start, range5.end), stmt->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, expressionStatement) {
    addToken(TokenType::String, "abc", range1);
    addToken(TokenType::Semicolon, range3);
    DIAG_NONE();
    ast::ExpressionStatement::Ptr stmt = expressionStatement();
    AST_CAST(ast::StringLiteral, expr, stmt->expression);
    EXPECT_EQ(SourceRange(range1.start, range3.end), stmt->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, expression) {
    addToken(TokenType::Integer, 1234, range1);
    addToken(TokenType::Plus);
    addToken(TokenType::String, "abc", range2);
    addToken(TokenType::Integer, 1111);
    DIAG_NONE();
    AST_CAST(ast::BinaryExpression, expr, expression());
    AST_CAST(ast::IntegerLiteral, left, expr->left);
    AST_CAST(ast::StringLiteral, right, expr->right);
    EXPECT_EQ(1234U, left->value);
    EXPECT_EQ("abc", right->value);
    EXPECT_EQ(SourceRange(range1.start, range2.end), expr->getRange());
    EXPECT_NOT_CONSUMED();
}

//a = b + c = d
//assign
//+-a
//+-assign
//  +-binary
//    +-b
//    +-c
//  +-d
TEST_F(ParserImplTest, assignment) {
    addToken(TokenType::Identifier, "a", range1);
    addToken(TokenType::Assign, range2);
    addToken(TokenType::String, "b", range3);
    addToken(TokenType::Plus, range4);
    addToken(TokenType::Identifier, "c", range5);
    addToken(TokenType::Assign, range6);
    addToken(TokenType::String, "d", range7);
    addToken(TokenType::CurlyLeft);
    DIAG_NONE();
    AST_CAST(ast::Assignment, assign1, expression());
    AST_CAST(ast::Identifier, var1, assign1->left);
    AST_CAST(ast::Assignment, assign2, assign1->right);
    AST_CAST(ast::BinaryExpression, bin, assign2->left);
    AST_CAST(ast::StringLiteral, var4, assign2->right);
    AST_CAST(ast::StringLiteral, var2, bin->left);
    AST_CAST(ast::Identifier, var3, bin->right);

    EXPECT_EQ("a", var1->name);
    EXPECT_EQ("b", var2->value);
    EXPECT_EQ("c", var3->name);
    EXPECT_EQ("d", var4->value);
    EXPECT_EQ(SourceRange(range1.start, range7.end), assign1->getRange());
    EXPECT_EQ(range2, assign1->operatorRange);
    EXPECT_EQ(SourceRange(range3.start, range7.end), assign2->getRange());
    EXPECT_EQ(range6, assign2->operatorRange);
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, additiveSingle) {
    addToken(TokenType::Integer, 1234, range1);
    addToken(TokenType::Integer, 1111, range2);
    DIAG_NONE();
    AST_CAST(ast::IntegerLiteral, expr, additiveExpression());
    EXPECT_EQ(1234U, expr->value);
    EXPECT_EQ(range1, expr->getRange());
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, additiveDouble) {
    addToken(TokenType::Integer, 1234, range1);
    addToken(TokenType::Plus, range2);
    addToken(TokenType::String, "abc", range3);
    addToken(TokenType::Integer, 1111);
    DIAG_NONE();
    AST_CAST(ast::BinaryExpression, expr, additiveExpression());
    AST_CAST(ast::IntegerLiteral, left, expr->left);
    AST_CAST(ast::StringLiteral, right, expr->right);
    EXPECT_EQ(1234U, left->value);
    EXPECT_EQ(range2, expr->operatorRange);
    EXPECT_EQ("abc", right->value);
    EXPECT_EQ(SourceRange(range1.start, range3.end), expr->getRange());
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, additiveTriple) {
    addToken(TokenType::Integer, 1234, range1);
    addToken(TokenType::Plus, range2);
    addToken(TokenType::String, "abc", range3);
    addToken(TokenType::Plus, range4);
    addToken(TokenType::Integer, 1111, range5);
    addToken(TokenType::Semicolon);
    DIAG_NONE();
    AST_CAST(ast::BinaryExpression, expr, additiveExpression());
    AST_CAST(ast::BinaryExpression, left, expr->left);
    AST_CAST(ast::IntegerLiteral, right, expr->right);
    AST_CAST(ast::IntegerLiteral, leftLeft, left->left);
    AST_CAST(ast::StringLiteral, leftRight, left->right);
    EXPECT_EQ(1234U, leftLeft->value);
    EXPECT_EQ(range2, left->operatorRange);
    EXPECT_EQ("abc", leftRight->value);
    EXPECT_EQ(range4, expr->operatorRange);
    EXPECT_EQ(1111U, right->value);
    EXPECT_EQ(SourceRange(range1.start, range3.end), left->getRange());
    EXPECT_EQ(SourceRange(range1.start, range5.end), expr->getRange());
    EXPECT_NOT_CONSUMED();
}

TEST_F(ParserImplTest, trim) {
    addToken(TokenType::KwTrim, 1234, range1);
    addToken(TokenType::Integer, 1111, range2);
    DIAG_NONE();
    AST_CAST(ast::UnaryExpression, expr, prefixExpression());
    EXPECT_EQ(range1, expr->operatorRange);
    AST_CAST(ast::IntegerLiteral, operand, expr->operand);
    EXPECT_EQ(SourceRange(range1.start, range2.end), expr->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, primaryInteger) {
    addToken(TokenType::Integer, 1234, range1);
    DIAG_NONE();
    AST_CAST(ast::IntegerLiteral, expr, primaryExpression());
    EXPECT_EQ(1234U, expr->value);
    EXPECT_EQ(range1, expr->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, primaryString) {
    addToken(TokenType::String, "abc", range1);
    DIAG_NONE();
    AST_CAST(ast::StringLiteral, expr, primaryExpression());
    EXPECT_EQ("abc", expr->value);
    EXPECT_EQ(range1, expr->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, primaryIdentifier) {
    addToken(TokenType::Identifier, "abc", range1);
    DIAG_NONE();
    AST_CAST(ast::Identifier, id, primaryExpression());
    EXPECT_EQ("abc", id->name);
    EXPECT_EQ(range1, id->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, primaryVarDecl) {
    addToken(TokenType::KwMy, range1);
    addToken(TokenType::Identifier, "abc", range2);
    DIAG_NONE();
    AST_CAST(ast::VarDecl, var, primaryExpression());
    EXPECT_EQ("abc", var->name);
    EXPECT_EQ(SourceRange(range1.start, range2.end), var->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, primaryFail) {
    addToken(TokenType::KwPrint, range1);
    DIAG_EXPECT(DiagId::ParserExpectedPrimaryExpression, 11, 12);
    AST_CAST(ast::IntegerLiteral, expr, primaryExpression());
    EXPECT_EQ(0U, expr->value);
    EXPECT_EQ(range1, expr->getRange());
    EXPECT_CONSUMED();
}

TEST_F(ParserImplTest, varDeclNoId) {
    addToken(TokenType::KwMy, range1);
    addToken(TokenType::String, "abc", range2);
    DIAG_EXPECT(DiagId::ParserExpectedVariableName, 21, 22);
    ast::VarDecl::Ptr var = varDecl();
    EXPECT_EQ("-error-", var->name);
    EXPECT_EQ(range1, var->getRange());
    EXPECT_NOT_CONSUMED();
}

} // namespace qore
