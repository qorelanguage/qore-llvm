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

DeclOrStmt Parser::parseDeclOrStmt() {
    LOG_FUNCTION();

    if (tokenType() == TokenType::EndOfFile) {
        return DeclOrStmt();
    }
    ast::Declaration::Ptr nsMember = namespaceMember(true);
    if (nsMember) {
        return DeclOrStmt(std::move(nsMember));
    }
    return DeclOrStmt(statement());
}

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
    ensureToken();
    script->start = token.location;
    while (tokenType() != TokenType::EndOfFile) {
        if (tokenType() == TokenType::KwModule) {
            //TODO module
            consume();
            match(TokenType::Identifier);
            match(TokenType::CurlyLeft);
            recoverSkipToCurlyRight();
            report(DiagId::ParserModuleIgnored);
            consume();
            continue;
        }
        ast::Declaration::Ptr nsMember = namespaceMember(true);
        if (nsMember) {
            script->members.push_back(std::move(nsMember));
        } else {
            script->statements.push_back(statement());
        }
    }
    script->end = token.location;
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

    ns->start = match(TokenType::KwNamespace).location;
    ns->modifiers = mods;
    ns->name = name();
    if (tokenType() == TokenType::CurlyLeft) {
        consume();
        while (tokenType() != TokenType::CurlyRight) {
            if (tokenType() == TokenType::EndOfFile) {
                report(DiagId::ParserUnendedNamespaceDecl);
                ns->end = token.location;
                return ns;
            }
            ast::Declaration::Ptr nsMember = namespaceMember(false);
            if (!nsMember) {
                ensureToken();
                report(DiagId::ParserExpectedNamespaceMember);
                recoverSkipToCurlyRight();
                break;
            }
            ns->members.push_back(std::move(nsMember));
        }
        ns->end = consume().location;
    } else {
        ns->end = match(TokenType::Semicolon, &Parser::recoverDoNothing).location;
    }
    return ns;
}

//namespace_member
//    : namespace_decl
//    | global_var_decl
//    | modifiers KW_CONST name '=' expr ';'
//    | modifiers type KW_SUB name param_list block
//    | modifiers KW_SUB name param_list block
//    | modifiers type name param_list block
//    | modifiers name param_list block
//    | class_def
//    ;
ast::Declaration::Ptr Parser::namespaceMember(bool topLevel) {
    LOG_FUNCTION();
    Recorder recorder(*this);
    ast::Modifiers mods = modifiers();
    switch (tokenType()) {
        case TokenType::KwNamespace:
            recorder.stop();
            return namespaceDecl(mods);
        case TokenType::KwOur: {
            recorder.stop();
            SourceLocation start = consume().location;
            ast::Type t = type();
            ast::Name n = name();
            SourceLocation end = match(TokenType::Semicolon).location;
            return ast::GlobalVariable::create(start, mods, std::move(t), std::move(n), end);
        }
        case TokenType::KwConst:
            recorder.stop();
            return constant(mods);
        case TokenType::KwClass:
            recorder.stop();
            return classDecl(mods);
        case TokenType::KwSub: {
            ast::Type t = ast::Type::createImplicit(location());
            consume();
            if (tokenType() == TokenType::DoubleColon || tokenType() == TokenType::Identifier) {
                recorder.stop();
                ast::Name n = name();
                return ast::Function::create(std::move(n), routine(mods, std::move(t)));
            }
            //no name after the sub keyword -> must be a closure
            break;
        }
        case TokenType::Asterisk: {
            ast::Type t = type();
            if (tokenType() == TokenType::KwSub) {
                consume();
                if (tokenType() == TokenType::DoubleColon || tokenType() == TokenType::Identifier) {
                    recorder.stop();
                    ast::Name n = name();
                    return ast::Function::create(std::move(n), routine(mods, std::move(t)));
                }
                //no name after the sub keyword -> must be a closure
            } else if (tokenType() == TokenType::DoubleColon || tokenType() == TokenType::Identifier) {
                //identifier after type -> either variable declaration or function without the sub keyword
                ast::Name n = name();
                if (tokenType() == TokenType::ParenLeft) {
                    recorder.stop();
                    return ast::Function::create(std::move(n), routine(mods, std::move(t)));
                }
                //must be a variable declaration
            }
            break;
        }
        case TokenType::DoubleColon:
        case TokenType::Identifier: {
            //might be:
            // (1) return type of a function with the sub keyword
            // (2) return type of a closure
            // (3) return type of a function without the sub keyword
            // (4) variable declaration
            // (5) method invocation
            // (6) a function name (with implicit return type)
            // (7) variable access
            SourceLocation start = location();
            ast::Name nOrT = name();

            if (tokenType() == TokenType::KwSub) {              //(1) or (2)
                consume();
                if (tokenType() == TokenType::DoubleColon || tokenType() == TokenType::Identifier) {
                                                                //(1) - function with the sub keyword and type
                    recorder.stop();
                    ast::Name n = name();
                    return ast::Function::create(std::move(n), routine(mods, ast::Type::createBasic(std::move(nOrT))));
                }
                //no name after the sub keyword -> must be a closure (2)
            } else if (tokenType() == TokenType::DoubleColon || tokenType() == TokenType::Identifier) {  // (3) or (4)
                ast::Name n = name();
                if (tokenType() == TokenType::ParenLeft) {      // (3) - function without the sub keyword, with type
                    recorder.stop();
                    return ast::Function::create(std::move(n), routine(mods, ast::Type::createBasic(std::move(nOrT))));
                }
                //two names not followed by '(' -> variable declaration (4)
            } else if (tokenType() == TokenType::ParenLeft) {   // (5) or (6)
                if (!topLevel) {
                    //method invocation (5) is not allowed here
                    //must be a function with implicit type without the sub keyword (6)
                    recorder.stop();
                    return ast::Function::create(std::move(nOrT), routine(mods, ast::Type::createImplicit(start)));
                }
                //we can't tell unless we look at the token after the matching ')' and if it is a '{',
                //we still don't know (function body vs. member access)
                //so we don't allow typeless subless top level functions and assume (5)
            } else {
                //must be variable access (7)
            }
            break;
        }
        default:
            break;
    }
    recorder.rewind();
    return nullptr;
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
