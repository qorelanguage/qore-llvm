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
#include "qore/comp/Scanner.h"
#include "qore/common/Logging.h"

/// \cond IGNORED_BY_DOXYGEN
#define REPORT(diagId)  diagMgr.report(DiagId::diagId, src.getMarkLocation())
/// \endcond

namespace qore {
namespace comp {

const std::unordered_map<std::string, TokenType> Scanner::Keywords{
    { "catch",          TokenType::KwCatch },
    { "else",           TokenType::KwElse },
    { "if",             TokenType::KwIf },
    { "my",             TokenType::KwMy },
    { "print",          TokenType::KwPrint },
    { "trim",           TokenType::KwTrim },
    { "try",            TokenType::KwTry },
};

Scanner::Scanner(DiagManager &diagMgr) : diagMgr(diagMgr) {
    LOG_FUNCTION();
}

Token Scanner::read(Source &src) {
    LOG_FUNCTION();

    while (true) {
        while (isWhitespace(src.peek()) || isNewline(src.peek())) {
            src.read();
        }

        src.setMark();
        TokenType type = readInternal(src);

        if (type != TokenType::None) {
            LOG("Returning token " << type);
            return Token(type, src.getMarkLocation(), src.getMarkedLength());
        }
    }
}

TokenType Scanner::readInternal(Source &src) {
    LOG_FUNCTION();
    switch (char c = src.read()) {
        case '\0':
            return TokenType::EndOfFile;
        case '+':
            return TokenType::Plus;
        case ';':
            return TokenType::Semicolon;
        case '{':
            return TokenType::CurlyLeft;
        case '}':
            return TokenType::CurlyRight;
        case '(':
            return TokenType::ParenLeft;
        case ')':
            return TokenType::ParenRight;
        case '=':
            return TokenType::Assign;
        case '#':
            readLineComment(src);
            return TokenType::None;
        case '/':
            if (src.peek() == '*') {
                readBlockComment(src);
                return TokenType::None;
            }
            REPORT(ScannerInvalidCharacter) << c;
            return TokenType::None;
        case '%':
            if (src.wasFirstOnLine()) {
                return readParseDirective(src);
            }
            REPORT(ScannerInvalidCharacter) << c;
            return TokenType::None;
        case '"':
        case '\'':
            src.unread();
            readString(src);
            return TokenType::String;
        case '0':   case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
            return readInteger(src);
        case 'a':   case 'b':   case 'c':   case 'd':   case 'e':
        case 'f':   case 'g':   case 'h':   case 'i':   case 'j':
        case 'k':   case 'l':   case 'm':   case 'n':   case 'o':
        case 'p':   case 'q':   case 'r':   case 's':   case 't':
        case 'u':   case 'v':   case 'w':   case 'x':   case 'y': case 'z':
        case 'A':   case 'B':   case 'C':   case 'D':   case 'E':
        case 'F':   case 'G':   case 'H':   case 'I':   case 'J':
        case 'K':   case 'L':   case 'M':   case 'N':   case 'O':
        case 'P':   case 'Q':   case 'R':   case 'S':   case 'T':
        case 'U':   case 'V':   case 'W':   case 'X':   case 'Y': case 'Z':
        case '_':
            return readIdentifier(src);
        default:
            REPORT(ScannerInvalidCharacter) << c;
            return TokenType::None;
    }
}

TokenType Scanner::readIdentifier(Source &src) {
    while (isalnum(src.peek()) || src.peek() == '_') {
        src.read();
    }
    auto it = Keywords.find(src.getMarkedString());
    return it == Keywords.end() ? TokenType::Identifier : it->second;
}

TokenType Scanner::readInteger(Source &src) {
    while (isdigit(src.peek())) {
        src.read();
    }
    return TokenType::Integer;
}

void Scanner::readString(Source &src) {
    int type = src.read();
    assert(type == '\'' || type == '"');
    bool escape = false;
    while (true) {
        char c = src.read();
        if (c == '\0') {
            REPORT(ScannerUnendedStringLiteral);
            break;
        }
        if (c == type && !escape) {
            break;
        }
        escape = c == '\\' && type == '"' && !escape;
    }
}

void Scanner::readLineComment(Source &src) {
    while (!isNewline(src.peek()) && src.peek() != '\0') {
        src.read();
    }
}

void Scanner::readBlockComment(Source &src) {
    src.read();         //consume opening asterisk
    bool asterisk = false;
    while (true) {
        int c = src.read();
        if (c == '\0') {
            REPORT(ScannerUnendedBlockComment);
            return;
        }
        if (c == '/' && asterisk) {
            return;
        }
        asterisk = c == '*';
    }
}

TokenType Scanner::readParseDirective(Source &src) {
    while (src.peek() != '\0' && !isNewline(src.peek()) && !isWhitespace(src.peek())) {
        src.read();
    }
    return TokenType::ParseDirective;
}

} //namespace comp
} //namespace qore
