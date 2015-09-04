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
#include "qore/common/Logging.h"

namespace qore {

ast::Program::Ptr ParserImpl::parse() {
    LOG_FUNCTION();
    return program();
}

ast::Statement::Ptr ParserImpl::parseStatement() {
    LOG_FUNCTION();
    return tokenType() == TokenType::EndOfFile ? nullptr : statement();
}

//program ::= statements
ast::Program::Ptr ParserImpl::program() {
    LOG_FUNCTION();
    return ast::Program::create(statements());
}

//statements
//    ::= *lambda*
//    ::= statements statement
ast::Statements ParserImpl::statements() {
    LOG_FUNCTION();
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
    LOG_FUNCTION();
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

//printStatement ::= KwPrint expression ';'
ast::PrintStatement::Ptr ParserImpl::printStatement() {
    LOG_FUNCTION();
    match(TokenType::KwPrint);
    ast::Expression::Ptr expr = expression();
    match(TokenType::Semicolon, &ParserImpl::recoverStatementEnd);
    return ast::PrintStatement::create(std::move(expr));
}

//expression ::= additiveExpression
ast::Expression::Ptr ParserImpl::expression() {
    LOG_FUNCTION();
    return additiveExpression();
}

//additiveExpression ::= primaryExpression additiveExpressionRest
//additiveExpressionRest
//    ::= *lambda*
//    ::= '+' primaryExpression additiveExpressionRest
ast::Expression::Ptr ParserImpl::additiveExpression() {
    LOG_FUNCTION();
    std::unique_ptr<ast::Expression> expr = primaryExpression();
    while (tokenType() == TokenType::Plus) {
        consume();
        expr = ast::BinaryExpression::create(std::move(expr), primaryExpression());
    }
    return expr;
}

//primaryExpression
//    ::= Number
//    ::= String
ast::Expression::Ptr ParserImpl::primaryExpression() {
    LOG_FUNCTION();
    switch (tokenType()) {
        case TokenType::Integer:
            return ast::IntegerLiteral::create(consumeIntValue());
        case TokenType::String:
            return ast::StringLiteral::create(consumeStringValue());
        default:
            report(DiagId::ParserExpectedPrimaryExpression) << token;
            recoverConsumeToken();
            //TODO return special error node which will prevent further errors
            return ast::IntegerLiteral::create(0);
    }
}

} // namespace qore
