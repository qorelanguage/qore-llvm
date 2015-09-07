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
/// \private
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_PARSER_PARSERCORE_H_
#define INCLUDE_QORE_PARSER_PARSERCORE_H_

#include "qore/context/DiagManager.h"
#include "qore/scanner/Scanner.h"

namespace qore {

/**
 * \private
 */
class ParserCore {

protected:
    using RecoverStrategy = void (ParserCore::*)();

protected:
    ParserCore(DiagManager &diagMgr, Scanner &scanner) : diagMgr(diagMgr), scanner(scanner), token(), hasToken(false) {
    }

    SourceRange match(TokenType expected, RecoverStrategy recoverStrategy = &ParserCore::recoverConsumeToken) {
        if (tokenType() == expected) {
            return consume().range;
        }
        report(DiagId::ParserUnexpectedToken) << to_string(expected) << token;
        (this->*recoverStrategy)();
        return token.range;
    }

    void recoverConsumeToken() {
        consume();
    }

    void recoverStatementEnd() {
        //TODO right curly brace & other tokens
        DisableDiag dd(diagMgr);
        while (true) {
            switch (tokenType()) {
                case TokenType::EndOfFile:
                    return;
                case TokenType::Semicolon:
                    consume();
                    return;
                default:
                    consume();
                    break;
            }
        }
    }

    void ensureToken() {
        if (!hasToken) {
            scanner.read(&token);
            hasToken = true;
        }
    }

    TokenType tokenType() {
        ensureToken();
        return token.type;
    }

    SourceRange range() {
        ensureToken();
        return token.range;
    }

    Token consume() {
        assert(hasToken);
        hasToken = false;
        CLOG("PARSER", "Consuming" << token);
        return token;
    }

    DiagBuilder report(DiagId id) {
        assert(hasToken);
        return diagMgr.report(id, token.range.start);
    }

protected:
    DiagManager &diagMgr;
    Scanner &scanner;
    Token token;
    bool hasToken;
};

} // namespace qore

#endif // INCLUDE_QORE_PARSER_PARSERCORE_H_
