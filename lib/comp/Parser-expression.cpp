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
/// \brief Parser implementation - expressions
///
//------------------------------------------------------------------------------
#include "qore/comp/Parser.h"

namespace qore {
namespace comp {

ast::Expression::Ptr Parser::expression() {
    LOG_FUNCTION();

    return assignmentExpr();
}

ast::Expression::Ptr Parser::assignmentExpr() {
    LOG_FUNCTION();

    return primaryExpr();
}

ast::Expression::Ptr Parser::primaryExpr() {
    LOG_FUNCTION();

    switch (tokenType()) {
        case TokenType::KwFalse:
        case TokenType::KwNothing:
        case TokenType::KwNull:
        case TokenType::KwSelf:
        case TokenType::KwTrue:
        case TokenType::Integer:
            return ast::LiteralExpression::create(consume());
        case TokenType::Identifier:
            return ast::NameExpression::create(name());
        case TokenType::ParenLeft:
            return parenExpr();
        case TokenType::CurlyLeft:
            return curlyExpr();
        default:
            report(DiagId::ParserExpectedPrimaryExpression) << util::to_string(tokenType());
            return ast::ErrorExpression::create(consume());
    }
}

//primary_expr
//    | '(' expr ')'
//    | '(' hash ')'
//    | '(' ')'
ast::Expression::Ptr Parser::parenExpr() {
    Token lParen = match(TokenType::ParenLeft);
    if (tokenType() == TokenType::ParenRight) {
        return ast::ListExpression::create(lParen, ast::ListExpression::Data(), consume());
    }
    ast::Expression::Ptr expr = expression();
    if (tokenType() == TokenType::Colon) {
        return hash(lParen, std::move(expr));
    }
    match(TokenType::ParenRight);
    return expr;
}

//primary_expr
//    | '{' '}'
//    | '{' hash '}'
ast::Expression::Ptr Parser::curlyExpr() {
    Token lCurly = match(TokenType::CurlyLeft);
    if (tokenType() == TokenType::CurlyRight) {
        return ast::HashExpression::create(lCurly, ast::HashExpression::Data(), consume());
    }
    return hash(lCurly, expression());
}

//hash
//    : hash_element
//    | hash ',' hash_element
//    | hash ','
//    ;
//
//hash_element
//    : expr ':' assignment_expr
//    ;
ast::Expression::Ptr Parser::hash(Token openToken, ast::Expression::Ptr expr) {

    ast::HashExpression::Data data;
    TokenType end = openToken.type == TokenType::ParenLeft ? TokenType::ParenRight : TokenType::CurlyRight;

    while (true) {
        match(TokenType::Colon);
        ast::Expression::Ptr value = assignmentExpr();
        data.emplace_back(std::move(expr), std::move(value));

        if (tokenType() != TokenType::Comma) {
            break;
        }
        consume();
        if (tokenType() == end) {
            break;
        }
        expr = expression();
    }
    return ast::HashExpression::create(openToken, std::move(data), match(end));
}

//name
//    : IDENTIFIER
//    | IDENTIFIER '::' name
//    ;
ast::Name Parser::name() {
    ast::Name name;
    name.tokens.push_back(match(TokenType::Identifier));
    while (tokenType() == TokenType::DoubleColon) {
        consume();
        name.tokens.push_back(match(TokenType::Identifier));
    }
    return name;
}

} // namespace comp
} // namespace qore
