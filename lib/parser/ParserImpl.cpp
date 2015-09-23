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

//block ::= '{' statements '}
ast::CompoundStatement::Ptr ParserImpl::block() {
    LOG_FUNCTION();
    SourceLocation start = match(TokenType::CurlyLeft).start;
    ast::Statements statements;
    while (tokenType() != TokenType::CurlyRight) {
        statements.push_back(statement());
    }
    SourceLocation end = match(TokenType::CurlyRight).end;
    return ast::CompoundStatement::create(SourceRange(start, end), std::move(statements));
}

//statement
//    ::= ';'
//    ::= printStatement
//    ::= ifStatement
//    ::= expressionStatement
ast::Statement::Ptr ParserImpl::statement() {
    LOG_FUNCTION();
    switch (tokenType()) {
        case TokenType::Semicolon:
            return ast::EmptyStatement::create(consume().range);
        case TokenType::KwPrint:
            return printStatement();
        case TokenType::KwIf:
            return ifStatement();
        case TokenType::CurlyLeft:
            return block();
        default:
            return expressionStatement();
    }
}

//ifStatement
//    ::= KwIf '(' expression ')' statement
//    ::= KwIf '(' expression ')' statement KwElse statement
ast::IfStatement::Ptr ParserImpl::ifStatement() {
    LOG_FUNCTION();
    SourceLocation start = match(TokenType::KwIf).start;
    match(TokenType::ParenLeft);
    ast::Expression::Ptr condition = expression();
    match(TokenType::ParenRight);
    ast::Statement::Ptr thenBranch = statement();
    ast::Statement::Ptr elseBranch;
    SourceLocation end;
    if (tokenType() == TokenType::KwElse) {
        consume();
        elseBranch = statement();
        end = elseBranch->getRange().end;
    } else {
        end = thenBranch->getRange().end;
        elseBranch = ast::EmptyStatement::create(SourceRange(end, end));
    }
    return ast::IfStatement::create(SourceRange(start, end), std::move(condition),
            std::move(thenBranch), std::move(elseBranch));
}

//printStatement ::= KwPrint expression ';'
ast::PrintStatement::Ptr ParserImpl::printStatement() {
    LOG_FUNCTION();
    SourceLocation start = match(TokenType::KwPrint).start;
    ast::Expression::Ptr expr = expression();
    SourceLocation end = match(TokenType::Semicolon, &ParserImpl::recoverStatementEnd).end;
    return ast::PrintStatement::create(SourceRange(start, end), std::move(expr));
}

//expressionStatement ::= expression ';'
ast::ExpressionStatement::Ptr ParserImpl::expressionStatement() {
    LOG_FUNCTION();
    ast::Expression::Ptr expr = expression();
    SourceLocation start = expr->getRange().start;
    SourceLocation end = match(TokenType::Semicolon, &ParserImpl::recoverStatementEnd).end;
    return ast::ExpressionStatement::create(SourceRange(start, end), std::move(expr));
}

//expression ::= assignment
ast::Expression::Ptr ParserImpl::expression() {
    LOG_FUNCTION();
    return assignment();
}

//assignment ::= additiveExpression '=' assignment
ast::Expression::Ptr ParserImpl::assignment() {
    LOG_FUNCTION();
    std::unique_ptr<ast::Expression> expr = additiveExpression();
    if (tokenType() == TokenType::Assign) {
        SourceRange r = consume().range;
        expr = ast::Assignment::create(std::move(expr), r, assignment());
    }
    return expr;
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
//    ::= varDecl
ast::Expression::Ptr ParserImpl::primaryExpression() {
    LOG_FUNCTION();
    SourceRange r = range();
    switch (tokenType()) {
        case TokenType::Integer:
            return ast::IntegerLiteral::create(r, consume().intValue);
        case TokenType::String:
            return ast::StringLiteral::create(r, consume().stringValue);
        case TokenType::Identifier:
            return ast::Identifier::create(r, consume().stringValue);
        case TokenType::KwMy:
            return varDecl();
        default:
            report(DiagId::ParserExpectedPrimaryExpression) << token;
            recoverConsumeToken();
            //TODO return special error node which will prevent further errors
            return ast::IntegerLiteral::create(r, 0);
    }
}

//varDec ::= KwMy Identifier
ast::VarDecl::Ptr ParserImpl::varDecl() {
    LOG_FUNCTION();
    SourceRange r = match(TokenType::KwMy);
    if (tokenType() != TokenType::Identifier) {
        report(DiagId::ParserExpectedVariableName) << token;
        //TODO return special error node which will prevent further errors
        return ast::VarDecl::create(r, "-error-");
    }
    r.end = range().end;
    return ast::VarDecl::create(r, consume().stringValue);
}

} // namespace qore
