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
/// \brief Parser implementation - statements
///
//------------------------------------------------------------------------------
#include "qore/comp/Parser.h"

namespace qore {
namespace comp {

ast::Statement::Ptr Parser::statement() {
    LOG_FUNCTION();
    switch (tokenType()) {
        case TokenType::Semicolon:
            return ast::EmptyStatement::create(consume().location);
        case TokenType::CurlyLeft:
            return compoundStmt();
        case TokenType::KwReturn:
            return returnStmt();
        case TokenType::KwIf:
            return ifStmt();
        case TokenType::KwTry:
            return tryStmt();
        case TokenType::KwForeach:
            return foreachStmt();
        case TokenType::KwThrow:
            return throwStmt();
        case TokenType::KwRethrow:
        case TokenType::KwThreadExit:
        case TokenType::KwBreak:
        case TokenType::KwContinue: {
            ast::SimpleStatement::Ptr stmt = ast::SimpleStatement::create();
            stmt->keyword = consume();
            stmt->end = match(TokenType::Semicolon).location;
            return std::move(stmt);
        }
        default:
            return expressionStmt();
    }
    return ast::Statement::Ptr();
}

ast::ExpressionStatement::Ptr Parser::expressionStmt() {
    LOG_FUNCTION();
    ast::ExpressionStatement::Ptr stmt = ast::ExpressionStatement::create();
    stmt->start = location();
    stmt->expression = expression();
    stmt->end = match(TokenType::Semicolon, &Parser::recoverSkipToSemicolon).location;
    return stmt;
}

ast::CompoundStatement::Ptr Parser::compoundStmt() {
    LOG_FUNCTION();
    ast::CompoundStatement::Ptr block = ast::CompoundStatement::create();
    block->start = match(TokenType::CurlyLeft).location;
    while (tokenType() != TokenType::CurlyRight && tokenType() != TokenType::EndOfFile) {
        block->statements.push_back(statement());
    }
    block->end = match(TokenType::CurlyRight).location;
    return block;
}

ast::ReturnStatement::Ptr Parser::returnStmt() {
    LOG_FUNCTION();
    ast::ReturnStatement::Ptr stmt = ast::ReturnStatement::create();
    stmt->start = match(TokenType::KwReturn).location;
    if (tokenType() != TokenType::Semicolon) {
        stmt->expression = expression();
    }
    stmt->end = match(TokenType::Semicolon, &Parser::recoverSkipToSemicolon).location;
    return stmt;
}

ast::IfStatement::Ptr Parser::ifStmt() {
    LOG_FUNCTION();
    ast::IfStatement::Ptr stmt = ast::IfStatement::create();
    stmt->start = match(TokenType::KwIf).location;
    match(TokenType::ParenLeft);
    stmt->condition = expression();
    match(TokenType::ParenRight);
    stmt->stmtTrue = statement();
    if (tokenType() == TokenType::KwElse) {
        consume();
        stmt->stmtFalse = statement();
    }
    return stmt;
}

ast::TryStatement::Ptr Parser::tryStmt() {
    LOG_FUNCTION();
    ast::TryStatement::Ptr stmt = ast::TryStatement::create();
    stmt->start = match(TokenType::KwTry).location;
    stmt->stmtTry = statement();
    match(TokenType::KwCatch);
    match(TokenType::ParenLeft);
    stmt->exceptionType = type();
    stmt->exceptionName = match(TokenType::Identifier);
    match(TokenType::ParenRight);
    stmt->stmtCatch = statement();
    return stmt;
}

ast::ForeachStatement::Ptr Parser::foreachStmt() {
    LOG_FUNCTION();
    ast::ForeachStatement::Ptr stmt = ast::ForeachStatement::create();
    stmt->start = match(TokenType::KwForeach).location;
    stmt->varType = type();
    stmt->varName = match(TokenType::Identifier);
    match(TokenType::KwIn);
    match(TokenType::ParenLeft);        //XXX needed because '{' would be interpreted as IndexExpression
    stmt->expr = expression();
    match(TokenType::ParenRight);
    stmt->stmt = statement();
    return stmt;
}

ast::ThrowStatement::Ptr Parser::throwStmt() {
    LOG_FUNCTION();
    ast::ThrowStatement::Ptr stmt = ast::ThrowStatement::create();
    stmt->start = match(TokenType::KwThrow).location;
    while (true) {
        stmt->exprList.push_back(expression());
        if (tokenType() == TokenType::Comma) {
            consume();
        } else {
            break;
        }
    }
    stmt->end = match(TokenType::Semicolon).location;
    return stmt;
}

} // namespace comp
} // namespace qore
