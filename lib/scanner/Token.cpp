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
/// \brief Token functions.
///
//------------------------------------------------------------------------------
#include "qore/scanner/Token.h"
#include <sstream>
#include "qore/common/Util.h"

namespace qore {

Token::operator std::string() const {
    std::ostringstream stream;
    stream << *this;
    return stream.str();
}

std::string to_string(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::None: return "None";
        case TokenType::EndOfFile: return "EndOfFile";
        case TokenType::KwPrint: return "KwPrint";
        case TokenType::Integer: return "Integer";
        case TokenType::String: return "String";
        case TokenType::Plus: return "Plus";
        case TokenType::Semicolon: return "Semicolon";
    }
    QORE_UNREACHABLE("unknown token type " << static_cast<int>(tokenType));
}

std::ostream &operator<<(std::ostream &o, const Token &token) {
    o << '{' << token.type;
    if (token.type == TokenType::Integer) {
        o << ", intValue = " << token.intValue;
    }
    if (token.type == TokenType::String) {
        o << ", stringValue = " << token.stringValue;
    }
    return o << "}";
}

} // namespace qore
