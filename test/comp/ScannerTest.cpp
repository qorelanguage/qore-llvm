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
#include "qore/comp/SourceManager.h"
#include "MockDiagProcessor.h"

namespace qore {
namespace comp {

class ScannerTest : public qtif::LineTest {
};

TEST_P(ScannerTest, Run) {
    Scanner scanner(diagMgr);
    while (true) {
        Token token = scanner.read(getSrc(), ITokenStream::Mode::Normal);
        output << token.type << token.location << ':' << token.length << '\n';
        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }
}

QTIF_TEST_CASE(ScannerTest, "scanner/([^/]*$)|(mode/normal)");

class ScannerModeSimpleRegexTest : public qtif::LineTest {
};

TEST_P(ScannerModeSimpleRegexTest, Run) {
    Scanner scanner(diagMgr);
    while (true) {
        Token token = scanner.read(getSrc(), ITokenStream::Mode::NormalOrSimpleRegex);
        output << token.type << token.location << ':' << token.length << '\n';
        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }
}

QTIF_TEST_CASE(ScannerModeSimpleRegexTest, "scanner/mode/simple_regex");

class ScannerModeRegexTest : public qtif::LineTest {
};

TEST_P(ScannerModeRegexTest, Run) {
    Scanner scanner(diagMgr);
    while (true) {
        Token token = scanner.read(getSrc(), ITokenStream::Mode::Regex);
        output << token.type << token.location << ':' << token.length << '\n';
        if (token.type == TokenType::EndOfFile) {
            break;
        }
    }
}

QTIF_TEST_CASE(ScannerModeRegexTest, "scanner/mode/regex");

class ScannerCoverageTest : public ::testing::Test, public DiagManagerHelper {
};

TEST_F(ScannerCoverageTest, InvalidCharacter) {
    Scanner scanner(diagMgr);
    SourceManager srcMgr(diagMgr, "");
    Source &src = srcMgr.createFromString("", "\x7f;");

    EXPECT_CALL(mockDiagProcessor, process(MatchDiagRecordId(DiagId::ScannerInvalidCharacter))).Times(1);
    EXPECT_EQ(TokenType::Semicolon, scanner.read(src, ITokenStream::Mode::Normal).type);
}

} // namespace comp
} // namespace qore
