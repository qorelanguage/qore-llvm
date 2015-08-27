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
/// \brief Defines the interface of the scanner.
///
//------------------------------------------------------------------------------

#ifndef INCLUDE_QORE_SCANNER_SCANNERINTERFACE_H_
#define INCLUDE_QORE_SCANNER_SCANNERINTERFACE_H_

#include "qore/scanner/Token.h"

namespace qore {

/**
 * \brief Lexical analyzer which converts a stream of source characters into a stream of tokens.
 */
class Scanner {

public:
    virtual ~Scanner() = default;

    /**
     * \brief Reads the next token from the source script.
     * \param token the Token structure to be filled in
     */
    virtual void read(Token *token) = 0;

protected:
    Scanner() {
    }

private:
    Scanner(const Scanner &) = delete;
    Scanner(Scanner &&) = delete;
    Scanner &operator=(const Scanner &) = delete;
    Scanner &operator=(Scanner &&) = delete;
};

} //namespace qore

#endif /* INCLUDE_QORE_SCANNER_SCANNERINTERFACE_H_ */
