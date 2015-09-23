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
#include "gtest/gtest.h"
#include "qore/scanner/ScannerImpl.h"
#include "../Utils.h"
#include "../context/DiagTestHelper.h"
#include "../context/SourceTestHelper.h"

namespace qore {

struct ScannerImplTest : ::testing::Test, DiagTestHelper {
};

#define SCANNER(str)    std::string inputString = str; \
                SourceBuffer sourceBuffer = SourceTestHelper::createBuffer(inputString.begin(), inputString.end());  \
                ScannerImpl scanner(diagMgr, sourceBuffer);

TEST_F(ScannerImplTest, Eof) {
    Token t;
    SCANNER("");

    DIAG_NONE();
    scanner.read(&t);
    EXPECT_EQ(TokenType::EndOfFile, t.type);
    EXPECT_EQ(1, t.range.start.line);
    EXPECT_EQ(1, t.range.start.column);
    EXPECT_EQ(1, t.range.end.line);
    EXPECT_EQ(1, t.range.end.column);
}

TEST_F(ScannerImplTest, EofSkipSpaces) {
    Token t;
    SCANNER(" \t\n  ");

    DIAG_NONE();
    scanner.read(&t);
    EXPECT_EQ(TokenType::EndOfFile, t.type);
    EXPECT_EQ(2, t.range.start.line);
    EXPECT_EQ(3, t.range.start.column);
    EXPECT_EQ(2, t.range.end.line);
    EXPECT_EQ(3, t.range.end.column);
}

TEST_F(ScannerImplTest, Symbols) {
    Token t;
    SCANNER(" + ;{}=()");

    DIAG_NONE();
    scanner.read(&t);    EXPECT_EQ(TokenType::Plus, t.type);
    scanner.read(&t);    EXPECT_EQ(TokenType::Semicolon, t.type);
    scanner.read(&t);    EXPECT_EQ(TokenType::CurlyLeft, t.type);
    scanner.read(&t);    EXPECT_EQ(TokenType::CurlyRight, t.type);
    scanner.read(&t);    EXPECT_EQ(TokenType::Assign, t.type);
    scanner.read(&t);    EXPECT_EQ(TokenType::ParenLeft, t.type);
    scanner.read(&t);    EXPECT_EQ(TokenType::ParenRight, t.type);
}

TEST_F(ScannerImplTest, InvalidCharDiagAndRecover) {
    Token t;
    SCANNER("\n   @ \n+");

    DIAG_EXPECT_AND_CAPTURE(r, DiagId::ScannerInvalidCharacter, 2, 4);

    scanner.read(&t);
    EXPECT_EQ(TokenType::Plus, t.type);
    EXPECT_EQ(3, t.range.start.line);
    EXPECT_EQ(1, t.range.start.column);
    EXPECT_EQ(3, t.range.end.line);
    EXPECT_EQ(2, t.range.end.column);

    EXPECT_PRED2(contains, r.message, "@");
}

TEST_F(ScannerImplTest, Integer) {
    Token t;
    SCANNER(" 8 123");

    DIAG_NONE();

    scanner.read(&t);
    EXPECT_EQ(TokenType::Integer, t.type);
    EXPECT_EQ(8U, t.intValue);

    scanner.read(&t);
    EXPECT_EQ(TokenType::Integer, t.type);
    EXPECT_EQ(123U, t.intValue);
}

TEST_F(ScannerImplTest, IntegerMax) {
    Token t;
    SCANNER("18446744073709551615");

    DIAG_NONE();

    scanner.read(&t);
    EXPECT_EQ(TokenType::Integer, t.type);
    EXPECT_EQ(ULLONG_MAX, t.intValue);
}

TEST_F(ScannerImplTest, IntegerOverflowDiagAndRecover) {
    Token t;
    SCANNER("18446744073709551616 ;");

    DIAG_EXPECT_AND_CAPTURE(r, DiagId::ScannerInvalidInteger, 1, 1);

    scanner.read(&t);
    EXPECT_EQ(TokenType::Integer, t.type);
    EXPECT_EQ(0U, t.intValue);

    scanner.read(&t);
    EXPECT_EQ(TokenType::Semicolon, t.type);

    EXPECT_PRED2(contains, r.message, "18446744073709551616");
}

TEST_F(ScannerImplTest, KwPrint) {
    Token t;
    SCANNER("\n  print\n  ;");

    DIAG_NONE();

    scanner.read(&t);
    EXPECT_EQ(TokenType::KwPrint, t.type);
    EXPECT_EQ(2, t.range.start.line);
    EXPECT_EQ(3, t.range.start.column);
    EXPECT_EQ(2, t.range.end.line);
    EXPECT_EQ(8, t.range.end.column);
}

TEST_F(ScannerImplTest, Keyword) {
    Token t;
    SCANNER("else if my trim");
    DIAG_NONE();
    scanner.read(&t); EXPECT_EQ(TokenType::KwElse, t.type);
    scanner.read(&t); EXPECT_EQ(TokenType::KwIf, t.type);
    scanner.read(&t); EXPECT_EQ(TokenType::KwMy, t.type);
    scanner.read(&t); EXPECT_EQ(TokenType::KwTrim, t.type);
}

TEST_F(ScannerImplTest, Identifier) {
    Token t;
    SCANNER("\nprn print");

    DIAG_NONE();

    scanner.read(&t);
    EXPECT_EQ(TokenType::Identifier, t.type);
    EXPECT_EQ("prn", t.stringValue);
    EXPECT_EQ(2, t.range.start.line);
    EXPECT_EQ(1, t.range.start.column);
    EXPECT_EQ(2, t.range.end.line);
    EXPECT_EQ(4, t.range.end.column);
}

TEST_F(ScannerImplTest, String) {
    Token t;
    SCANNER(" \"xyz\"  ;");

    DIAG_NONE();

    scanner.read(&t);
    EXPECT_EQ(TokenType::String, t.type);
    EXPECT_EQ("xyz", t.stringValue);
    EXPECT_EQ(1, t.range.start.line);
    EXPECT_EQ(2, t.range.start.column);
    EXPECT_EQ(1, t.range.end.line);
    EXPECT_EQ(7, t.range.end.column);
}

TEST_F(ScannerImplTest, StringUnendedNewline) {
    Token t;
    SCANNER(" \"xyz\n  ;");

    DIAG_EXPECT_AND_CAPTURE(r, DiagId::ScannerUnendedStringLiteral, 1, 2);

    scanner.read(&t);
    EXPECT_EQ(TokenType::String, t.type);
    EXPECT_EQ("xyz", t.stringValue);

    scanner.read(&t);
    EXPECT_EQ(TokenType::Semicolon, t.type);
}

TEST_F(ScannerImplTest, StringUnendedEof) {
    Token t;
    SCANNER(" \"xyz");

    DIAG_EXPECT_AND_CAPTURE(r, DiagId::ScannerUnendedStringLiteral, 1, 2);

    scanner.read(&t);
    EXPECT_EQ(TokenType::String, t.type);
    EXPECT_EQ("xyz", t.stringValue);

    scanner.read(&t);
    EXPECT_EQ(TokenType::EndOfFile, t.type);
}

} // namespace qore
