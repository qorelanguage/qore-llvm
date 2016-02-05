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
/// \brief Parser implementation - script and namespaces
///
//------------------------------------------------------------------------------
#include "qore/comp/Parser.h"

namespace qore {
namespace comp {

//script
//    : script_member
//    | script script_member
//    ;
//
//script_member
//    : namespace_member
//    | KW_MODULE IDENTIFIER '{' module_decls '}'
//    | statement
//    ;
ast::Script::Ptr Parser::parseScript() {
    LOG_FUNCTION();

    ast::Script::Ptr script = ast::Script::create();
    while (tokenType() != TokenType::EndOfFile) {
//        if (tokenType() == TokenType::KwModule) {
//            //TODO module
//        }
        ast::NamespaceMember::Ptr nsMember = namespaceMember();
        if (nsMember) {
            script->members.push_back(std::move(nsMember));
        } else {
            //if (were modifiers) -> report error
            //statement();
        }
    }
    return script;
}

//namespace_decl
//    : modifiers KW_NAMESPACE IDENTIFIER '{' namespace_body '}'
//    | modifiers KW_NAMESPACE IDENTIFIER ';'
//    ;
//
//namespace_body
//    : /* empty */
//    | namespace_body namespace_member
//    ;
ast::Namespace::Ptr Parser::namespaceDecl(ast::Modifiers mods) {
    LOG_FUNCTION();
    ast::Namespace::Ptr ns = ast::Namespace::create();

    ns->location = match(TokenType::KwNamespace);
    ns->modifiers = mods;

    if (tokenType() == TokenType::Identifier) {
        ns->name = consume();
    } else {
        report(DiagId::ParserExpectedNamespaceName);
    }

    if (tokenType() == TokenType::CurlyLeft) {
        consume();
        while (tokenType() != TokenType::CurlyRight) {
            if (tokenType() == TokenType::EndOfFile) {
                report(DiagId::ParserUnendedNamespaceDecl);
                return ns;
            }
            ast::NamespaceMember::Ptr nsMember = namespaceMember();
            if (!nsMember) {
                report(DiagId::ParserExpectedNamespaceMember);
                recoverSkipToCurlyRight();
                return ns;
            }
            ns->members.push_back(std::move(nsMember));
        }
        consume();
    } else {
        match(TokenType::Semicolon, &Parser::recoverDoNothing);
    }
    return ns;
}

//namespace_member
//    : namespace_decl
//    | global_var_decl
//    | const_decl
//    | sub_def
//    | class_def
//    ;
ast::NamespaceMember::Ptr Parser::namespaceMember() {
    LOG_FUNCTION();
    ast::Modifiers mods = modifiers();
    switch (tokenType()) {
        case TokenType::KwNamespace:
            return namespaceDecl(mods);
        case TokenType::KwOur:
            //TODO return globalVarDecl(mods);
        case TokenType::KwConst:
            //TODO return constDecl(mods);
        case TokenType::KwClass:
            //TODO return classDecl(mods);
        case TokenType::KwSub:
            //TODO return subDef(mods, noType);
        default:
            //TODO ast::Type::Ptr type = typeOpt();
            return /*type ? subDef(mods, type) :*/ ast::NamespaceMember::Ptr();
    }
}

//modifiers
//    : /* empty */
//    | modifiers modifier
//    ;
//
//modifier
//    : KW_PRIVATE
//    | KW_STATIC
//    | KW_SYNCHRONIZED
//    | KW_DEPRECATED
//    | KW_PUBLIC
//    | KW_FINAL
//    | KW_ABSTRACT
//    ;
ast::Modifiers Parser::modifiers() {
    LOG_FUNCTION();
    ast::Modifiers mods;
    while (true) {
        ast::Modifier m;
        switch (tokenType()) {
            case TokenType::KwAbstract:     m = ast::Modifier::Abstract;        break;
            case TokenType::KwDeprecated:   m = ast::Modifier::Deprecated;      break;
            case TokenType::KwFinal:        m = ast::Modifier::Final;           break;
            case TokenType::KwPrivate:      m = ast::Modifier::Private;         break;
            case TokenType::KwPublic:       m = ast::Modifier::Public;          break;
            case TokenType::KwStatic:       m = ast::Modifier::Static;          break;
            case TokenType::KwSynchronized: m = ast::Modifier::Synchronized;    break;
            default:
                return mods;
        }
        if (mods.contains(m)) {
            report(DiagId::ParserModifierGivenTwice) << util::to_string(m);
        } else {
            mods |= m;
        }
        consume();
    }
}

} // namespace comp
} // namespace qore
