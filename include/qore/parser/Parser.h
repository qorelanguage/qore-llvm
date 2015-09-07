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
/// \brief Parser interface.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_PARSER_PARSER_H_
#define INCLUDE_QORE_PARSER_PARSER_H_

#include "qore/ast/Program.h"

namespace qore {

/**
 * \brief Qore parser interface.
 */
class Parser {

public:
    virtual ~Parser() = default;

    /**
     * \brief Parses the whole script.
     * \return the root of the AST
     */
    virtual ast::Program::Ptr parse() = 0;

    /**
     * \brief Parses a single statement.
     * \return the parsed statement or `nullptr` if the end of input has been reached
     */
    virtual ast::Statement::Ptr parseStatement() = 0;

protected:
    Parser() = default;

private:
    Parser(const Parser &) = delete;
    Parser(Parser &&) = delete;
    Parser &operator=(const Parser &) = delete;
    Parser &operator=(Parser &&) = delete;
};

} // namespace qore

#endif // INCLUDE_QORE_PARSER_PARSER_H_
