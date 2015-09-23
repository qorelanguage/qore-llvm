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

#include "qore/parser/Parser.h"
#include "qore/parser/ParserCore.h"

namespace qore {

/**
 * \brief Parser implementation.
 */
class ParserImpl : public Parser, private ParserCore {

public:
    /**
     * \brief Constructs the parser for given scanner.
     * \param diagMgr used for reporting diagnostic messages
     * \param scanner the source of tokens
     */
    ParserImpl(DiagManager &diagMgr, Scanner &scanner) : ParserCore(diagMgr, scanner) {
    }

    ast::Program::Ptr parse() override;
    ast::Statement::Ptr parseStatement() override;

private:
    ast::Program::Ptr program();
    ast::Statements statements();
    ast::CompoundStatement::Ptr block();
    ast::Statement::Ptr statement();
    ast::PrintStatement::Ptr printStatement();
    ast::IfStatement::Ptr ifStatement();
    ast::ExpressionStatement::Ptr expressionStatement();
    ast::Expression::Ptr expression();
    ast::Expression::Ptr assignment();
    ast::Expression::Ptr additiveExpression();
    ast::Expression::Ptr prefixExpression();
    ast::Expression::Ptr primaryExpression();
    ast::VarDecl::Ptr varDecl();

    friend class ParserImplTest;
};

} // namespace qore

#endif // INCLUDE_QORE_PARSER_PARSERIMPL_H_
