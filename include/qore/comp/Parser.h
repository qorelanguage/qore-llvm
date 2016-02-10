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
/// \brief Qore language parser.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_PARSER_H_
#define INCLUDE_QORE_COMP_PARSER_H_

#include "qore/common/Util.h"
#include "qore/comp/ast/Script.h"
#include "qore/comp/DiagManager.h"
#include "qore/comp/Token.h"

namespace qore {
namespace comp {

/**
 * \brief Qore language parser - converts a stream of tokens into an abstract syntax tree.
 */
class Parser {

public:
    /**
     * \brief Constructs the parser for given source of tokens.
     * \param diagMgr used for reporting diagnostic messages
     * \param src the source of tokens
     */
    Parser(DiagManager &diagMgr, ITokenStream &src) : diagMgr(diagMgr), src(src), hasToken(false) {
    }

    /**
     * \brief Parses the script.
     * \return parsed script
     */
    ast::Script::Ptr parseScript();

private:
    Parser(const Parser &) = delete;
    Parser(Parser &&) = delete;
    Parser &operator=(const Parser &) = delete;
    Parser &operator=(Parser &&) = delete;

private:
    using RecoverStrategy = void (Parser::*)();

    TokenType tokenType() {
        ensureToken();
        return token.type;
    }

    SourceLocation location() {
        ensureToken();
        return token.location;
    }

    Token consume() {
        assert(hasToken);
        hasToken = false;
        LOG("Consuming " << token.type);
        return token;
    }

    void ensureToken() {
        if (!hasToken) {
            token = src.read();
            hasToken = true;
        }
    }

    Token match(TokenType expected, RecoverStrategy recoverStrategy = &Parser::recoverConsumeToken) {
        if (tokenType() == expected) {
            return consume();
        }
        report(DiagId::ParserUnexpectedToken) << util::to_string(token.type) << util::to_string(expected);
        (this->*recoverStrategy)();
        return token;
    }

    DiagBuilder report(DiagId id) {
        assert(hasToken);
        return diagMgr.report(id, token.location);
    }

    void recoverDoNothing() {
    }

    void recoverConsumeToken() {
        consume();
    }

    void recoverSkipToCurlyRight() {
        int cnt = 0;
        DisableDiag dd(diagMgr);
        while (true) {
            switch (tokenType()) {
                case TokenType::EndOfFile:
                    return;
                case TokenType::CurlyLeft:
                    consume();
                    ++cnt;
                    break;
                case TokenType::CurlyRight:
                    consume();
                    if (cnt-- == 0) {
                        return;
                    }
                    break;
                default:
                    consume();
                    break;
            }
        }
    }

    void recoverSkipToSemicolon() {
        int cnt = 0;
        DisableDiag dd(diagMgr);
        while (true) {
            switch (tokenType()) {
                case TokenType::EndOfFile:
                    return;
                case TokenType::CurlyLeft:
                    consume();
                    ++cnt;
                    break;
                case TokenType::CurlyRight:
                    consume();
                    if (cnt-- == 0) {
                        return;
                    }
                    break;
                case TokenType::Semicolon:
                    consume();
                    if (cnt == 0) {
                        return;
                    }
                    break;
                default:
                    consume();
                    break;
            }
        }
    }

private:
    ast::Namespace::Ptr namespaceDecl(ast::Modifiers mods);
    ast::NamespaceMember::Ptr namespaceMember();
    ast::Modifiers modifiers();
    ast::Statement::Ptr statement();
    ast::ExpressionStatement::Ptr expressionStatement();
    ast::Expression::Ptr expression();
    ast::Expression::Ptr assignmentExpr();
    ast::Expression::Ptr primaryExpr();
    ast::Expression::Ptr parenExpr();
    ast::Expression::Ptr curlyExpr();
    ast::Expression::Ptr hash(Token openToken, ast::Expression::Ptr expr);
    ast::Name name();

private:
    DiagManager &diagMgr;
    ITokenStream &src;
    Token token;
    bool hasToken;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_PARSER_H_
