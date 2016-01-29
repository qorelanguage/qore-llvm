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
#include "../Qtif.h"
#include "qore/comp/Scanner.h"

namespace qore {
namespace comp {

class ScannerTest : public qtif::LineTest {
};

TEST_P(ScannerTest, Run) {
    Scanner scanner(diagMgr);
    while (true) {
        Token token = scanner.read(getSrc());
        output << token.type << token.location << ':' << token.length << '\n';
        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }
}

QTIF_TEST_CASE(ScannerTest, "scanner/detail");

class ScannerTestNoWs : public qtif::LineTest {
};

TEST_P(ScannerTestNoWs, Run) {
    Scanner scanner(diagMgr);
    while (true) {
        Token token = scanner.read(getSrc());
        if (token.type != TokenType::None && token.type != TokenType::Whitespace) {
            output << token.type << token.location << ':' << token.length << '\n';
        }
        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }
}

QTIF_TEST_CASE(ScannerTestNoWs, "scanner/nows-");

TEST(ScannerCoverage, Whitespace) {
    DiagManager diagMgr;
    Scanner scanner(diagMgr);
    std::string str = "\n\r\f\v \t";
    Source src("", 1, std::vector<char>(str.begin(), str.end()));

    Token t = scanner.read(src);
    EXPECT_EQ(TokenType::Whitespace, t.type);
    EXPECT_EQ(6, t.length);
    EXPECT_EQ(TokenType::EndOfFile, scanner.read(src).type);
}

} // namespace comp
} // namespace qore
