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

ScannerImpl::ScannerImpl(DiagManager &diagMgr, SourceBuffer sourceBuffer)
: diagMgr(diagMgr), sourceBuffer(std::move(sourceBuffer)), ptr(&this->sourceBuffer) {
    LOG_FUNCTION();
}

void ScannerImpl::read(Token *token) {
    LOG_FUNCTION();
    do {
        while (isspace(*ptr)) {
            ++ptr;
        }
        token->locationStart = ptr.getLocation();
    } while ((token->type = readInternal(token)) == TokenType::None);
    token->locationEnd = ptr.getLocation();
}

TokenType ScannerImpl::readInternal(Token *token) {
    LOG_FUNCTION();
    switch (char c = *ptr++) {
        case '\0':
            return TokenType::EndOfFile;
        case ';':
            return TokenType::Semicolon;
        case '0':   case '1':   case '2':   case '3':   case '4':
        case '5':   case '6':   case '7':   case '8':   case '9':
            return readInteger(token);
        default:
            diagMgr.report(DiagId::ScannerInvalidCharacter, token->locationStart) << c;
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
        diagMgr.report(DiagId::ScannerInvalidInteger, token->locationStart).arg(start, ptr);
        token->intValue = 0;
    }
    return TokenType::Integer;
}

} //namespace qore
