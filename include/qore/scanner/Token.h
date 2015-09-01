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
/// \brief Defines the structure of a token (lexical element).
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_SCANNER_TOKEN_H_
#define INCLUDE_QORE_SCANNER_TOKEN_H_

#include <ostream>
#include <string>
#include "qore/context/SourceLocation.h"

namespace qore {

/**
 * \brief Token types.
 */
enum class TokenType {
    None,           //!< Represents no token. Used for error recovery.
    EndOfFile,      //!< A token reported at the end of the source.
    KwPrint,        //!< The `print` keyword.
    Integer,        //!< An integer literal.
    String,         //!< A string literal.
    Plus,           //!< The '+' symbol.
    Semicolon,      //!< The ';' symbol.
};

/**
 * \brief Describes a token (lexical element).
 */
struct Token {
    TokenType type{TokenType::None};    //!< The type of the token.
    SourceLocation locationStart;       //!< Location of the start of the token in the source code.
    SourceLocation locationEnd;         //!< Location of the end of the token in the source code.
    uint64_t intValue;                  //!< An integer value associated with the token.
    std::string stringValue;

    operator std::string() const;
};

/**
 * \brief Converts TokenType to its textual representation.
 * \param tokenType the token type
 * \return name of the token type
 */
std::string to_string(TokenType tokenType);

/**
 * \brief Dumps the name of the token type to an output stream.
 * \param o the output stream
 * \param tokenType the token type
 * \return the output stream for chaining calls
 */
inline std::ostream &operator<<(std::ostream &o, TokenType tokenType) {
    return o << to_string(tokenType);
}

/**
 * \brief Dumps information about a token to an output stream.
 * \param o the output stream
 * \param token the token to dump
 * \return the output stream for chaining calls
 */
std::ostream &operator<<(std::ostream &o, const Token &token);

} //namespace qore

#endif /* INCLUDE_QORE_SCANNER_TOKEN_H_ */
