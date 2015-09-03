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
/// \brief Parser implementation.
///
//------------------------------------------------------------------------------
#include "qore/parser/ParserImpl.h"

namespace qore {

ast::Program::Ptr ParserImpl::parse() {
    return program();
}

ast::Statement::Ptr ParserImpl::parseStatement() {
    if (tokenType() == TokenType::EndOfFile) {
        return nullptr;
    }
    return statement();
}

//program ::= statements
ast::Program::Ptr ParserImpl::program() {
    return ast::Program::create(statements());
}

//statements
//    ::= *lambda*
//    ::= statements statement
ast::Statements ParserImpl::statements() {
    ast::Statements body;
    while (tokenType() != TokenType::EndOfFile) {
        body.push_back(statement());
    }
    return body;
}

//statement
//    ::= ';'
//    ::= printStatement
ast::Statement::Ptr ParserImpl::statement() {
    switch (tokenType()) {
        case TokenType::Semicolon:
            consume();
            return ast::EmptyStatement::create();
        case TokenType::KwPrint:
            return printStatement();
        default:
            report(DiagId::ParserStatementExpected) << token;
            recoverStatementEnd();
            return ast::EmptyStatement::create();
    }
}

//printStatement ::= KW_PRINT expression ';'
ast::PrintStatement::Ptr ParserImpl::printStatement() {
    match(TokenType::KwPrint);
    ast::Expression::Ptr expr = expression();
    match(TokenType::Semicolon, &ParserImpl::recoverStatementEnd);
    return ast::PrintStatement::create(std::move(expr));
}

//expression
//    ::= additiveExpression
ast::Expression::Ptr ParserImpl::expression() {
    return additiveExpression();
}

//additiveExpression
//    ::= primaryExpression
//    ::= additiveExpression '+' primaryExpression
//
//without left recursion:
//
//additiveExpression ::= primaryExpression additiveExpressionRest
//additiveExpressionRest
//    ::= *lambda*
//    ::= '+' primaryExpression additiveExpressionRest
ast::Expression::Ptr ParserImpl::additiveExpression() {
    std::unique_ptr<ast::Expression> expr = primaryExpression();
    while (tokenType() == TokenType::Plus) {
        consume();
        expr = ast::BinaryExpression::create(std::move(expr), primaryExpression());
    }
    return expr;
}

//primaryExpression
//    | NUMBER                        { $$ = new IntegerLiteral($1); }
//    | STRING                        { $$ = new StringLiteral($1); }
ast::Expression::Ptr ParserImpl::primaryExpression() {
    switch (tokenType()) {
        case TokenType::Integer:
            return ast::IntegerLiteral::create(consumeIntValue());
        case TokenType::String:
            return ast::StringLiteral::create(consumeStringValue());
        default:
            report(DiagId::ParserExpectedPrimaryExpression) << token;
            recoverConsumeToken();
            return ast::IntegerLiteral::create(0);       //TODO return special error node which will prevent further errors
    }
}

//TODO logging!!!

} // namespace qore
