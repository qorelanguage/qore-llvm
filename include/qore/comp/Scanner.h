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
/// \brief Scanner implementation.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SCANNER_H_
#define INCLUDE_QORE_COMP_SCANNER_H_

#include <unordered_map>
#include "qore/comp/DiagManager.h"
#include "qore/comp/Source.h"
#include "qore/comp/Token.h"

namespace qore {
namespace comp {

/**
 * \brief Lexical analyzer which converts a stream of source characters into a stream of tokens.
 */
class Scanner {

public:
    /**
     * \brief Constructs the scanner.
     * \param diagMgr used for reporting diagnostic messages
     */
    explicit Scanner(DiagManager &diagMgr);

    /**
     * \brief Reads the next token from the source script.
     * \param src the source to read characters from
     * \param mode determines the type of tokens expected
     * \return the token read from the source
     */
    Token read(Source &src, ITokenStream::Mode mode);

    /**
     * \brief Reads a string literal from a source.
     *
     * Consumes all characters that are part of the string literal up to and including the closing quote.
     * \param src the source which must be positioned at the opening single or double quote character
     */
    void readString(Source &src);

    /**
     * \brief Reads a line comment from the source.
     *
     * Consumes all characters that are part of the comment, but leaves the terminating new line character.
     * \param src the source which must be positioned after the '#' character
     */
    void readLineComment(Source &src);

    /**
     * \brief Reads a block comment from the source.
     *
     * Consumes all characters that are part of the comment
     * \param src the source which must be positioned between the the opening '/' and '*' characters
     */
    void readBlockComment(Source &src);

    /**
     * \brief Determines whether a character is considered horizontal whitespace.
     * \param c the character to test
     * \return `true` if `c` is either a space or a horizontal tab character
     */
    static bool isWhitespace(int c) {
        return c == ' ' || c == '\t';
    }

    /**
     * \brief Determines whether a character is considered a new-line character.
     * \param c the character to test
     * \return `true` if `c` is either CR or LF
     */
    static bool isNewline(int c) {
        return c == '\r' || c == '\n';
    }

    /**
     * \brief Determines whether a character is considered a hexadecimal digit.
     * \param c the character to test
     * \return `true` if `c` is 0-9, a-f or A-F
     */
    static bool isHexDigit(int c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    /**
     * \brief Determines whether a character is considered a modifier for a regular expression.
     * \param c the character to test
     * \return `true` if `c` is 'g', 'i', 's', 'x' or 'm'
     */
    static bool isRegexModifier(char c) {
        return c == 'g' || c == 'i' || c == 's' || c == 'x' || c == 'm';
    }

private:
    TokenType readInternal(Source &src, bool readRegex);
    TokenType readIdentifier(Source &src);
    TokenType readParseDirective(Source &src);
    TokenType readHexLiteral(Source &src);
    TokenType handleDigit(Source &src);
    TokenType readSimpleRegex(Source &src, int parts = 1);
    TokenType readRegex(Source &src);

private:
    Scanner(const Scanner &) = delete;
    Scanner(Scanner &&) = delete;
    Scanner &operator=(const Scanner &) = delete;
    Scanner &operator=(Scanner &&) = delete;

private:
    DiagManager &diagMgr;

    static const std::unordered_map<std::string, TokenType> Keywords;
};

} //namespace comp
} //namespace qore

#endif /* INCLUDE_QORE_COMP_SCANNER_H_ */
