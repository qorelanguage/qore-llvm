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
#include "qore/common/Util.h"

/// \cond IGNORED_BY_DOXYGEN
#define REPORT(diagId)  diagMgr.report(DiagId::diagId, src.getMarkLocation())
/// \endcond

namespace qore {
namespace comp {

const std::unordered_map<std::string, TokenType> Scanner::KeywordsAndDirectives{
    { "catch",          TokenType::KwCatch },
    { "else",           TokenType::KwElse },
    { "if",             TokenType::KwIf },
    { "my",             TokenType::KwMy },
    { "print",          TokenType::KwPrint },
    { "trim",           TokenType::KwTrim },
    { "try",            TokenType::KwTry },
    { "%include",       TokenType::PdInclude },
};

Scanner::Scanner(DiagManager &diagMgr) : diagMgr(diagMgr) {
    LOG_FUNCTION();
}

Token Scanner::read(Source &src) {
    LOG_FUNCTION();

    src.setMark();
    TokenType type = readInternal(src);

    LOG("Returning token " << type);
    return Token(type, src.getMarkLocation(), src.getMarkedLength());
}

std::string Scanner::readDirectiveParam(Source &src) {
    std::string s;

    if (src.peek() != '\0' && !isspace(src.peek())) {
        //TODO require whitespace between directive name and parameter?
    }

    while (true) {
        int c = src.read();

        switch (c) {
            case '#':
                readLineComment(src);
                break;
            case '/':
                if (src.peek() == '*') {
                    readBlockComment(src);
                    s.push_back(' ');
                } else {
                    s.push_back(c);
                }
                break;
            case '\'':
            case '"':
                s.push_back(c);
                src.setMark();
                readString(src, c);
                s.append(src.getMarkedString());
                break;
            case '\0':
            case '\r':
            case '\n':
                return util::trim<>(s, isspace);
            default:
                s.push_back(c);
                break;
        }
    }
}

TokenType Scanner::readInternal(Source &src) {
    LOG_FUNCTION();
    switch (char c = src.read()) {
        case '\0':
            return TokenType::EndOfFile;
        case ' ':   case '\t':  case '\n':  case '\v':  case '\f':  case '\r':
            while (isspace(src.peek())) {
                src.read();
            }
            return TokenType::Whitespace;
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
        case '%':
            if (src.wasFirstOnLine()) {
                return readParseDirective(src);
            }
            REPORT(ScannerInvalidCharacter) << c;
            return TokenType::None;
        case '#':
            return readLineComment(src);
        case '/':
            if (src.peek() == '*') {
                return readBlockComment(src);
            }
            REPORT(ScannerInvalidCharacter) << c;
            return TokenType::None;
        case '"':
        case '\'':
            return readString(src, c);
/*
        case '0':   case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
            return readInteger(token);*/
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
    auto it = KeywordsAndDirectives.find(src.getMarkedString());
    return it == KeywordsAndDirectives.end() ? TokenType::Identifier : it->second;
}

TokenType Scanner::readParseDirective(Source &src) {
    while (isalnum(src.peek()) || src.peek() == '-') {
        src.read();
    }
    auto it = KeywordsAndDirectives.find(src.getMarkedString());
    if (it != KeywordsAndDirectives.end()) {
        return it->second;
    }
    REPORT(PdpUnknownDirective) << src.getMarkedString();
    return TokenType::None;
}

TokenType Scanner::readString(Source &src, char type) {
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
    return TokenType::String;
}

TokenType Scanner::readLineComment(Source &src) {
    while (src.peek() != '\r' && src.peek() != '\n' && src.peek() != '\0') {
        src.read();
    }
    return TokenType::Comment;
}

TokenType Scanner::readBlockComment(Source &src) {
    src.read();         //consume opening asterisk
    bool asterisk = false;
    while (true) {
        int c = src.read();
        if (c == '\0') {
            REPORT(PdpUnendedBlockComment);
            return TokenType::Comment;
        }
        if (c == '/' && asterisk) {
            return TokenType::Comment;
        }
        asterisk = c == '*';
    }
}

} //namespace comp
} //namespace qore
