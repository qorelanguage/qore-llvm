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
    return ast::Program::create(statements(), token.range);
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
            return ast::EmptyStatement::create(consume().range);
        case TokenType::KwPrint:
            return printStatement();
        default:
            SourceLocation start = token.range.start;
            report(DiagId::ParserStatementExpected) << token;
            recoverStatementEnd();
            return ast::EmptyStatement::create(SourceRange(start, token.range.end));
    }
}

//printStatement ::= KwPrint expression ';'
ast::PrintStatement::Ptr ParserImpl::printStatement() {
    LOG_FUNCTION();
    SourceLocation start = match(TokenType::KwPrint).start;
    ast::Expression::Ptr expr = expression();
    SourceLocation end = match(TokenType::Semicolon, &ParserImpl::recoverStatementEnd).end;
    return ast::PrintStatement::create(SourceRange(start, end), std::move(expr));
}

//expression ::= additiveExpression
ast::Expression::Ptr ParserImpl::expression() {
    LOG_FUNCTION();
    return additiveExpression();
}

//additiveExpression ::= prefixExpression additiveExpressionRest
//additiveExpressionRest
//    ::= *lambda*
//    ::= '+' prefixExpression additiveExpressionRest
ast::Expression::Ptr ParserImpl::additiveExpression() {
    LOG_FUNCTION();
    std::unique_ptr<ast::Expression> expr = prefixExpression();
    while (tokenType() == TokenType::Plus) {
        SourceRange r = consume().range;
        expr = ast::BinaryExpression::create(std::move(expr), r, prefixExpression());
    }
    return expr;
}

//prefixExpression
//    ::= primaryExpression
//    ::= KwTrim prefixExpression
ast::Expression::Ptr ParserImpl::prefixExpression() {
    LOG_FUNCTION();
    if (tokenType() == TokenType::KwTrim) {
        SourceRange r = consume().range;
        return ast::UnaryExpression::create(r, prefixExpression());
    } else {
        return primaryExpression();
    }
}

//primaryExpression
//    ::= Number
//    ::= String
ast::Expression::Ptr ParserImpl::primaryExpression() {
    LOG_FUNCTION();
    SourceRange r = range();
    switch (tokenType()) {
        case TokenType::Integer:
            return ast::IntegerLiteral::create(consume().intValue, r);
        case TokenType::String:
            return ast::StringLiteral::create(consume().stringValue, r);
        default:
            report(DiagId::ParserExpectedPrimaryExpression) << token;
            recoverConsumeToken();
            //TODO return special error node which will prevent further errors
            return ast::IntegerLiteral::create(0, r);
    }
}

} // namespace qore
