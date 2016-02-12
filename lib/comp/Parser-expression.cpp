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

    return shiftExpr();
}

//shift_expr
//    : add_expr
//    | shift_expr SHIFT_LEFT add_expr
//    | shift_expr SHIFT_RIGHT add_expr
//    ;
ast::Expression::Ptr Parser::shiftExpr() {
    LOG_FUNCTION();
    ast::Expression::Ptr e = addExpr();
    while (tokenType() == TokenType::DoubleAngleLeft || tokenType() == TokenType::DoubleAngleRight) {
        Token t = consume();
        e = ast::BinaryExpression::create(std::move(e), t, addExpr());
    }
    return e;
}

//add_expr
//    : mult_expr
//    | add_expr '+' mult_expr
//    | add_expr '-' mult_expr
//    ;
ast::Expression::Ptr Parser::addExpr() {
    LOG_FUNCTION();
    ast::Expression::Ptr e = multExpr();
    while (tokenType() == TokenType::Plus || tokenType() == TokenType::Minus) {
        Token t = consume();
        e = ast::BinaryExpression::create(std::move(e), t, multExpr());
    }
    return e;
}

//mult_expr
//    : prefix_expr
//    | mult_expr '*' prefix_expr
//    | mult_expr '/' prefix_expr
//    | mult_expr '%' prefix_expr
//    ;
ast::Expression::Ptr Parser::multExpr() {
    LOG_FUNCTION();
    ast::Expression::Ptr e = prefixExpr();
    while (tokenType() == TokenType::Asterisk || tokenType() == TokenType::Slash || tokenType() == TokenType::Percent) {
        Token t = consume();
        e = ast::BinaryExpression::create(std::move(e), t, prefixExpr());
    }
    return e;
}

//prefix_expr
//    : postfix_expr
//    | '+' prefix_expr
//    | '-' prefix_expr
//    | '~' prefix_expr
//    | '!' prefix_expr
//    | '\\' prefix_expr
//    | '++' prefix_expr
//    | '--' prefix_expr
//    | KW_ELEMENTS prefix_expr
//    | KW_KEYS prefix_expr
//    | KW_SHIFT prefix_expr
//    | KW_POP prefix_expr
//    | KW_CHOMP prefix_expr
//    | KW_TRIM prefix_expr
//    | KW_BACKGROUND prefix_expr
//    | KW_DELETE prefix_expr
//    | KW_REMOVE prefix_expr
//    ;
ast::Expression::Ptr Parser::prefixExpr() {
    LOG_FUNCTION();
    Token t;
    switch (tokenType()) {
        case TokenType::Plus:
        case TokenType::Minus:
        case TokenType::Tilde:
        case TokenType::Exclamation:
        case TokenType::Backslash:
        case TokenType::DoublePlus:
        case TokenType::DoubleMinus:
        case TokenType::KwElements:
        case TokenType::KwKeys:
        case TokenType::KwShift:
        case TokenType::KwPop:
        case TokenType::KwChomp:
        case TokenType::KwTrim:
        case TokenType::KwBackground:
        case TokenType::KwDelete:
        case TokenType::KwRemove:
            t = consume();
            return ast::UnaryExpression::create(prefixExpr(), t, false);
        default:
            return postfixExpr();
    }
}

//postfix_expr
//    : primary_expr
//    | postfix_expr arg_list
//    | postfix_expr '[' expr ']'
//    | postfix_expr '{' expr '}'
//    | postfix_expr '.' IDENTIFIER
//    | postfix_expr '.' literal      //anything that can be converted to string
//    | postfix_expr '.' '(' expr ')'
//    | postfix_expr P_INCREMENT
//    | postfix_expr P_DECREMENT
//    ;
ast::Expression::Ptr Parser::postfixExpr() {
    LOG_FUNCTION();
    ast::Expression::Ptr e = primaryExpr();
    ast::Expression::Ptr index;
    while (true) {
        switch (tokenType()) {
            case TokenType::ParenLeft:
                e = ast::CallExpression::create(std::move(e), argList());
                break;
            case TokenType::SquareLeft:
                consume();
                index = expression();
                e = ast::IndexExpression::create(std::move(e), std::move(index), match(TokenType::SquareRight));
                break;
            case TokenType::CurlyLeft:
                consume();
                index = expression();
                e = ast::IndexExpression::create(std::move(e), std::move(index), match(TokenType::CurlyRight));
                break;
            case TokenType::Dot:
                consume();
                if (tokenType() == TokenType::ParenLeft) {
                    consume();
                    index = expression();
                    e = ast::IndexExpression::create(std::move(e), std::move(index), match(TokenType::ParenRight));
                } else if (tokenType() == TokenType::Identifier) {
                    e = ast::AccessExpression::create(std::move(e), consume());
                } else {
                    //TODO e."abc", e.3, etc.
                    report(DiagId::ParserInvalidMemberAccess) << util::to_string(tokenType());
                }
                break;
            case TokenType::DoubleMinus:
            case TokenType::DoublePlus:
                e = ast::UnaryExpression::create(std::move(e), consume(), true);
                break;
            default:
                return e;
        }
    }
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
            if (tokenType() == TokenType::Comma) {
                return list(lParen, std::move(expr));
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
        case TokenType::KwNew: {
            Token t = consume();
            ast::Name n = name();
            return ast::NewExpression::create(t.location, std::move(n), argList());
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
        data.emplace_back(std::move(expr), expression());

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
//    | '(' expr_list ',' ')'
//    | '(' expr_list ')'
ast::Expression::Ptr Parser::list(Token openToken, ast::Expression::Ptr expr) {
    LOG_FUNCTION();

    ast::ListExpression::Data data;
    data.emplace_back(std::move(expr));
    while (tokenType() == TokenType::Comma) {
        consume();

        if (tokenType() == TokenType::ParenRight) {
            break;
        }
        data.emplace_back(expression());
    }
    return ast::ListExpression::create(openToken, std::move(data), match(TokenType::ParenRight));
}

//primary_expr
//    | modifiers type KwSub param_list block
//    | modifiers KwSub param_list block
ast::ClosureExpression::Ptr Parser::closure(ast::Modifiers mods, ast::Type::Ptr type) {
    LOG_FUNCTION();
    match(TokenType::KwSub);
    return ast::ClosureExpression::create(routine(mods, std::move(type), ast::Name()));
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

//arg_list
//    : '(' ')'
//    | '(' expr_list ')'
//    ;
//expr_list
//    : expr
//    | expr_list ',' expr
//    ;
ast::ArgList::Ptr Parser::argList() {
    LOG_FUNCTION();

    ast::ArgList::Data data;
    Token openToken = match(TokenType::ParenLeft);
    if (tokenType() != TokenType::ParenRight) {
        data.emplace_back(expression());
        while (tokenType() == TokenType::Comma) {
            consume();
            data.emplace_back(expression());
        }
    }
    return ast::ArgList::create(openToken, std::move(data), match(TokenType::ParenRight));
}

} // namespace comp
} // namespace qore
