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
        case TokenType::KwAbstract:
        case TokenType::KwDeprecated:
        case TokenType::KwFinal:
        case TokenType::KwPrivate:
        case TokenType::KwPublic:
        case TokenType::KwStatic:
        case TokenType::KwSynchronized: {
            ast::Modifiers mods = modifiers();
            if (tokenType() == TokenType::KwSub) {
                return closure(mods, ast::ImplicitType::create(location()));
            }
            return closure(mods, type());
        }
        case TokenType::KwSub:
            return closure(ast::Modifiers(), ast::ImplicitType::create(location()));
        case TokenType::Asterisk: {
            ast::Type::Ptr t = type();
            if (tokenType() == TokenType::KwSub) {
                return closure(ast::Modifiers(), std::move(t));
            }
            return ast::VarDeclExpression::create(std::move(t), match(TokenType::Identifier));
        }
        case TokenType::Identifier: {
            ast::Name n = name();
            if (tokenType() == TokenType::KwSub) {
                return closure(ast::Modifiers(), ast::NameType::create(std::move(n)));
            }
            if (tokenType() == TokenType::Identifier) {
                return ast::VarDeclExpression::create(ast::NameType::create(std::move(n)), consume());
            }
            return ast::NameExpression::create(std::move(n));
        }
        case TokenType::ParenLeft: {
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
        case TokenType::CurlyLeft: {
            Token lCurly = match(TokenType::CurlyLeft);
            if (tokenType() == TokenType::CurlyRight) {
                return ast::HashExpression::create(lCurly, ast::HashExpression::Data(), consume());
            }
            return hash(lCurly, expression());
        }
        case TokenType::KwCast: {
            ast::CastExpression::Ptr cast = ast::CastExpression::create();
            cast->start = consume().location;
            match(TokenType::AngleLeft);
            cast->type = type();
            match(TokenType::AngleRight);
            match(TokenType::ParenLeft);
            cast->expression = expression();
            cast->end = match(TokenType::ParenRight).location;
            return std::move(cast);
        }
        default:
            report(DiagId::ParserExpectedPrimaryExpression) << util::to_string(tokenType());
            return ast::ErrorExpression::create(consume());
    }
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
    LOG_FUNCTION();

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

//primary_expr
//    | modifiers type KwSub param_list block
//    | modifiers KwSub param_list block
ast::ClosureExpression::Ptr Parser::closure(ast::Modifiers mods, ast::Type::Ptr type) {
    LOG_FUNCTION();
    match(TokenType::KwSub);
    ast::Routine::Ptr r = ast::Routine::create();
    r->modifiers = mods;
    r->type = std::move(type);
    r->params = paramList();
    r->body = compoundStmt();
    return ast::ClosureExpression::create(std::move(r));
}

//param_list
//    : '(' ')'
//    | '(' params ')'
//    ;
//
//params
//    : param
//    | param ',' params
//    ;
//
//param
//    : type IDENTIFIER
//    | type IDENTIFIER '=' expr
//    ;
std::vector<ast::Routine::Param> Parser::paramList() {
    LOG_FUNCTION();
    std::vector<ast::Routine::Param> params;
    match(TokenType::ParenLeft);
    if (tokenType() != TokenType::ParenRight && tokenType() != TokenType::EndOfFile) {
        while (true) {
            ast::Type::Ptr t = type();
            Token id = match(TokenType::Identifier);
            ast::Expression::Ptr expr;
            if (tokenType() == TokenType::Equals) {
                consume();
                expr = expression();
            }
            params.emplace_back(std::move(t), id, std::move(expr));
            if (tokenType() != TokenType::Comma) {
                break;
            }
            consume();
        }
    }
    match(TokenType::ParenRight);
    return params;
}

//name
//    : IDENTIFIER
//    | IDENTIFIER '::' name
//    ;
ast::Name Parser::name() {
    LOG_FUNCTION();
    ast::Name name;
    name.tokens.push_back(match(TokenType::Identifier));
    while (tokenType() == TokenType::DoubleColon) {
        consume();
        name.tokens.push_back(match(TokenType::Identifier));
    }
    return name;
}

//type
//    : name
//    | '*' name
//    ;
ast::Type::Ptr Parser::type() {
    LOG_FUNCTION();
    if (tokenType() == TokenType::Asterisk) {
        SourceLocation l = consume().location;
        return ast::AsteriskType::create(l, name());
    }
    return ast::NameType::create(name());
}

} // namespace comp
} // namespace qore
