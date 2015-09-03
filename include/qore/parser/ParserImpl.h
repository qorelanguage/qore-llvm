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
/// \brief Parser implementation.
///
//------------------------------------------------------------------------------

#ifndef INCLUDE_QORE_PARSER_PARSERIMPL_H_
#define INCLUDE_QORE_PARSER_PARSERIMPL_H_

#include "qore/context/DiagManager.h"
#include "qore/parser/Parser.h"
#include "qore/scanner/Scanner.h"

namespace qore {

class ParserImpl : public Parser {

public:
    /**
     * \brief Constructs the parser for given scanner.
     * \param scanner the input stream representing the source to tokenize
     */
    ParserImpl(DiagManager &diagMgr, Scanner &scanner);

    ast::Program::Ptr parse() override;
    ast::Statement::Ptr parseStatement() override;

private:
    void match(TokenType expected, void (ParserImpl::*recoverStrategy)() = &ParserImpl::recoverConsumeToken);

    void recoverDoNothing();
    void recoverConsumeToken();
    void recoverStatementEnd();

    ast::Program::Ptr program();
    ast::Statements statements();
    ast::Statement::Ptr statement();
    ast::PrintStatement::Ptr printStatement();
    ast::Expression::Ptr expression();
    ast::Expression::Ptr additiveExpression();
    ast::Expression::Ptr primaryExpression();

private:
    DiagManager &diagMgr;
    Scanner &scanner;

    Token tkn;
    bool empty{true};

    TokenType tokenType() {
        if (empty) {
            scanner.read(&tkn);
            empty = false;
        }
        return tkn.type;
    }

    void consume() {
        assert(!empty);
        empty = true;
    }

    uint64_t consumeIntValue() {
        assert(!empty);
        empty = true;
        return tkn.intValue;
    }

    std::string consumeStringValue() {
        assert(!empty);
        empty = true;
        return std::move(tkn.stringValue);
    }

    DiagBuilder report(DiagId id) {
        assert(!empty);
        return diagMgr.report(id, tkn.locationStart);
    }

    ParserImpl(const ParserImpl &) = delete;
    ParserImpl(ParserImpl &&) = delete;
    ParserImpl &operator=(const ParserImpl &) = delete;
    ParserImpl &operator=(ParserImpl &&) = delete;
};

} // namespace qore

#endif // INCLUDE_QORE_PARSER_PARSERIMPL_H_
