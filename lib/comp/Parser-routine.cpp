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
/// \brief Parser implementation - routines
///
//------------------------------------------------------------------------------
#include "qore/comp/Parser.h"
#include <vector>

namespace qore {
namespace comp {

ast::Routine::Ptr Parser::routine(ast::Modifiers mods, ast::Type type, bool method) {
    LOG_FUNCTION();
    ast::Routine::Ptr r = ast::Routine::create();
    r->modifiers = mods;
    r->type = std::move(type);
    r->params = paramList();
    if (method) {
        if (tokenType() == TokenType::Colon) {
            do {
                consume();
                ast::Name n = name();
                r->baseCtors.emplace_back(std::move(n), argList());
            } while (tokenType() == TokenType::Comma);
        }
        if (tokenType() == TokenType::Semicolon) {
            r->semicolon = consume().location;
            return r;
        }
    }
    r->body = compoundStmt();
    return r;
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
            ast::Type t = type();
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

} // namespace comp
} // namespace qore
