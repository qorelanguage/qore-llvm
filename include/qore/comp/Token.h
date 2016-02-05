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
#ifndef INCLUDE_QORE_COMP_TOKEN_H_
#define INCLUDE_QORE_COMP_TOKEN_H_

#include <ostream>
#include "qore/comp/SourceLocation.h"

namespace qore {
namespace comp {

/**
 * \brief Token types.
 *
 * \ref TokenData.inc
 */
enum class TokenType {
    None,           //!< Represents no token. Used for error recovery.
    #define TOK(N)       N,
    /// \cond IGNORED_BY_DOXYGEN
    #include "qore/comp/TokenData.inc"
    /// \endcond
    #undef TOK
};

/**
 * \brief Writes the name of the token type to an output stream.
 * \param o the output stream
 * \param tokenType the token type
 * \return the output stream for chaining calls
 */
std::ostream &operator<<(std::ostream &o, TokenType tokenType);

/**
 * \brief Describes a token (lexical element).
 */
struct Token {
    TokenType type;                     //!< The type of the token.
    SourceLocation location;            //!< Location of the token in the source code.
    int length;                         //!< Length of the token in characters.

    /**
     * \brief Default constructor.
     */
    Token() : type(TokenType::None), length(0) {
    }

    /**
     * \brief Constructs a token.
     * \param type the type of the token
     * \param location the location of the token
     * \param length the length of the token.
     */
    Token(TokenType type, SourceLocation location, int length) : type(type), location(location), length(length) {
    }
};

/**
 * \brief a stream of tokens.
 */
class ITokenStream {

public:
    virtual ~ITokenStream() = default;

    /**
     * \brief Reads the next token from the input, processing parse directives.
     * \return the next token read from the input
     */
    virtual Token read() = 0;

protected:
    ITokenStream() = default;

private:
    ITokenStream(const ITokenStream &) = delete;
    ITokenStream(ITokenStream &&) = delete;
    ITokenStream &operator=(const ITokenStream &) = delete;
    ITokenStream &operator=(ITokenStream &&) = delete;
};

} //namespace comp
} //namespace qore

#endif /* INCLUDE_QORE_COMP_TOKEN_H_ */
