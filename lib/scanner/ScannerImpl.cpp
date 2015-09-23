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
#include "qore/scanner/ScannerImpl.h"
#include <cstdlib>
#include "qore/common/Logging.h"

namespace qore {

ScannerImpl::ScannerImpl(DiagManager &diagMgr, SourcePointer sourcePointer)
: diagMgr(diagMgr), ptr(std::move(sourcePointer)) {
    LOG_FUNCTION();
}

void ScannerImpl::read(Token *token) {
    LOG_FUNCTION();

    token->stringValue.clear();
    do {
        while (isspace(*ptr)) {
            ++ptr;
        }
        token->range.start = ptr.getLocation();
    } while ((token->type = readInternal(token)) == TokenType::None);
    token->range.end = ptr.getLocation();
    LOG("Returning token " << *token);
}

TokenType ScannerImpl::readInternal(Token *token) {
    LOG_FUNCTION();
    switch (char c = *ptr++) {
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
        case '"':
            return readString(token);
        case '0':   case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
            return readInteger(token);
        case 'a':   case 'b':   case 'c':   case 'd':   case 'e':
        case 'f':   case 'g':   case 'h':   case 'i':   case 'j':
        case 'k':   case 'l':   case 'm':   case 'n':   case 'o':
        case 'p':   case 'q':   case 'r':   case 's':   case 't':
        case 'u':   case 'v':   case 'w':   case 'x':   case 'y':
        case 'z':
        case 'A':   case 'B':   case 'C':   case 'D':   case 'E':
        case 'F':   case 'G':   case 'H':   case 'I':   case 'J':
        case 'K':   case 'L':   case 'M':   case 'N':   case 'O':
        case 'P':   case 'Q':   case 'R':   case 'S':   case 'T':
        case 'U':   case 'V':   case 'W':   case 'X':   case 'Y':
        case 'Z':
            return readIdentifier(token);
        default:
            diagMgr.report(DiagId::ScannerInvalidCharacter, token->range.start) << c;
            return TokenType::None;
    }
}

TokenType ScannerImpl::readInteger(Token *token) {
    const char *start = ptr - 1;
    char *end;

    while (isdigit(*ptr)) {
        ++ptr;
    }

    errno = 0;
    token->intValue = strtoull(start, &end, 10);
    if (errno || end != ptr) {
        diagMgr.report(DiagId::ScannerInvalidInteger, token->range.start).arg(start, ptr);
        token->intValue = 0;
    }
    return TokenType::Integer;
}

TokenType ScannerImpl::readIdentifier(Token *token) {
    const char *start = ptr - 1;
    while (isalnum(*ptr)) {
        ++ptr;
    }
    const char *end = ptr;

    std::string s{start, end};
    //FIXME use something better
    if (s == "else") {
        return TokenType::KwElse;
    } else if (s == "if") {
        return TokenType::KwIf;
    } else if (s == "my") {
        return TokenType::KwMy;
    } else if (s == "print") {
        return TokenType::KwPrint;
    } else if (s == "trim") {
        return TokenType::KwTrim;
    }

    token->stringValue = std::move(s);
    return TokenType::Identifier;
}

TokenType ScannerImpl::readString(Token *token) {
    char c;
    while ((c = *ptr++) != '"') {
        if (c == '\0' || c == '\n') {
            diagMgr.report(DiagId::ScannerUnendedStringLiteral, token->range.start);
            break;
        }
        token->stringValue.push_back(c);
    }
    return TokenType::String;
}

} //namespace qore
