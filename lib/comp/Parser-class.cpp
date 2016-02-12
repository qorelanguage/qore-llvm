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
/// \brief Parser implementation - classes
///
//------------------------------------------------------------------------------
#include "qore/comp/Parser.h"

namespace qore {
namespace comp {

ast::Class::Ptr Parser::classDecl(ast::Modifiers mods) {
    LOG_FUNCTION();
    ast::Class::Ptr c = ast::Class::create();

    c->start = match(TokenType::KwClass).location;
    c->modifiers = mods;
    c->name = name();
    if (tokenType() == TokenType::KwInherits) {
        consume();
        ast::Modifiers m = modifiers();
        c->inherits.emplace_back(m, name());
        while (tokenType() == TokenType::Comma) {
            consume();
            ast::Modifiers m = modifiers();
            c->inherits.emplace_back(m, name());
        }
    }
    if (tokenType() == TokenType::CurlyLeft) {
        consume();
        while (tokenType() != TokenType::CurlyRight && tokenType() != TokenType::EndOfFile) {
            break;
//            c->members.push_back(classMember());
        }
        c->end = match(TokenType::CurlyRight).location;
    } else {
        c->end = match(TokenType::Semicolon, &Parser::recoverDoNothing).location;
    }
    return c;
}

} // namespace comp
} // namespace qore
