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
        do {
            consume();
            ast::Modifiers m = modifiers();
            c->inherits.emplace_back(m, name());
        } while (tokenType() == TokenType::Comma);
    }
    if (tokenType() == TokenType::CurlyLeft) {
        consume();
        while (tokenType() != TokenType::CurlyRight) {
            ast::Declaration::Ptr member = classMember();
            if (member) {
                c->members.push_back(std::move(member));
            } else {
                report(DiagId::ParserExpectedClassMember);
                recoverSkipToCurlyRight();
                break;
            }
        }
        c->end = consume().location;
    } else {
        c->end = match(TokenType::Semicolon, &Parser::recoverDoNothing).location;
    }
    return c;
}

ast::Declaration::Ptr Parser::classMember() {
    LOG_FUNCTION();
    ast::Modifiers mods = modifiers();
    switch (tokenType()) {
        case TokenType::CurlyLeft:
            if (mods.isEmpty()) {
                return ast::Declaration::Ptr();
            }
            return classMemberList(mods);
        case TokenType::Asterisk: {
            ast::Type t = type();
            ast::Name n = name();
            return ast::Method::create(std::move(n), routine(mods, std::move(t), true));
        }
        case TokenType::DoubleColon:
        case TokenType::Identifier: {
            ast::Name n = name();
            if (tokenType() == TokenType::DoubleColon || tokenType() == TokenType::Identifier) {
                ast::Type t = ast::Type::createBasic(std::move(n));
                n = name();
                return ast::Method::create(std::move(n), routine(mods, std::move(t), true));
            } else {
                ast::Type t = ast::Type::createImplicit(n.getStart());
                return ast::Method::create(std::move(n), routine(mods, std::move(t), true));
            }
        }
        default:
            return ast::Declaration::Ptr();
    }
}

ast::Declaration::Ptr Parser::classMemberList(ast::Modifiers groupMods) {
    LOG_FUNCTION();
    ast::MemberGroup::Ptr group = ast::MemberGroup::create();
    group->start = match(TokenType::CurlyLeft).location;
    group->modifiers = groupMods;

    while (tokenType() != TokenType::CurlyRight) {
        ast::Modifiers mods = modifiers();
        if (tokenType() == TokenType::KwConst) {
            group->members.push_back(constant(mods));
        } else {
            ast::Field::Ptr f = ast::Field::create();
            f->modifiers = mods;
            f->type = type();
            f->name = match(TokenType::Identifier);
            if (tokenType() == TokenType::Equals) {
                consume();
                f->exprInit = expression();
            } else if (tokenType() == TokenType::ParenLeft) {
                f->argListInit = argList();
            }
            f->end = match(TokenType::Semicolon).location;
            group->members.push_back(std::move(f));
        }
    }
    group->end = match(TokenType::CurlyRight).location;
    return std::move(group);
}

ast::Constant::Ptr Parser::constant(ast::Modifiers mods) {
    ast::Constant::Ptr c = ast::Constant::create();
    c->start = match(TokenType::KwConst).location;
    c->modifiers = mods;
    c->name = name();
    match(TokenType::Equals);
    c->initializer = expression();
    c->end = match(TokenType::Semicolon).location;
    return c;
}

} // namespace comp
} // namespace qore
