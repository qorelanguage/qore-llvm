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

ParserImpl::ParserImpl(DiagManager &diagMgr, Scanner &scanner) : diagMgr(diagMgr), scanner(scanner) {
}

void ParserImpl::match(TokenType expected, void (ParserImpl::*recoverStrategy)()) {
    if (tokenType() == expected) {
        consume();
    } else {
        report(DiagId::ParserUnexpectedToken) << to_string(expected) << tkn;
        (this->*recoverStrategy)();
    }
}

void ParserImpl::recoverDoNothing() {
}

void ParserImpl::recoverConsumeToken() {
    empty = true;
}

void ParserImpl::recoverStatementEnd() {
    //TODO right curly brace & other tokens
    DisableDiag dd(diagMgr);

    while (tokenType() != TokenType::Semicolon && tokenType() != TokenType::EndOfFile) {
        consume();
    }
}

std::unique_ptr<Program> ParserImpl::parse() {
    return program();
}

std::unique_ptr<Statement> ParserImpl::parseStatement() {
    if (tokenType() == TokenType::EndOfFile) {
        return nullptr;
    }
    return std::unique_ptr<Statement>(statement());
}

//program ::= statements
std::unique_ptr<Program> ParserImpl::program() {
    return std::unique_ptr<Program>(statements());
}

//statements
//    ::= *lambda*
//    ::= statements statement
Program *ParserImpl::statements() {
    ProgramBuilder builder;
    while (tokenType() != TokenType::EndOfFile) {
        builder.addStatement(statement());
    }
    return builder.build();
}

//statement
//    ::= ';'
//    ::= printStatement
Statement *ParserImpl::statement() {
    switch (tokenType()) {
        case TokenType::Semicolon:
            consume();
            return new EmptyStatement();
        case TokenType::KwPrint:
            return printStatement();
        default:
            report(DiagId::ParserStatementExpected) << tkn;
            recoverStatementEnd();
            return new EmptyStatement();
    }
}

//printStatement ::= KW_PRINT expression ';'
PrintStatement *ParserImpl::printStatement() {
    match(TokenType::KwPrint);
    Expression *expr = expression();
    match(TokenType::Semicolon, &ParserImpl::recoverStatementEnd);
    return new PrintStatement(expr);
}

//expression
//    ::= additiveExpression
Expression *ParserImpl::expression() {
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
Expression *ParserImpl::additiveExpression() {
    Expression *expr = primaryExpression();
    while (tokenType() == TokenType::Plus) {
        consume();
        expr = new BinaryExpression(expr, primaryExpression());
    }
    return expr;
}

//primaryExpression
//    | NUMBER                        { $$ = new IntegerLiteral($1); }
//    | STRING                        { $$ = new StringLiteral($1); }
Expression *ParserImpl::primaryExpression() {
    switch (tokenType()) {
        case TokenType::Integer: {
            return new IntegerLiteral(consumeIntValue());
        }
        case TokenType::String: {
            return new StringLiteral(consumeStringValue());
        }
        default:
            report(DiagId::ParserExpectedPrimaryExpression) << tkn;
            recoverConsumeToken();
            return new IntegerLiteral(0);       //TODO return special error node which will prevent further errors
    }
}

//TODO unique_ptr!!!
//TODO logging!!!

} // namespace qore
