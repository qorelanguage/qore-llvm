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
#include "MockDiagProcessor.h"
#include "qore/comp/DiagManager.h"

namespace qore {
namespace comp {

struct DiagManagerTest : ::testing::Test, DiagManagerHelper {
};

TEST_F(DiagManagerTest, Report) {
    DiagRecord record;
    EXPECT_CALL(mockDiagProcessor, process(::testing::_)).WillOnce(::testing::SaveArg<0>(&record));

    diagMgr.report(DiagId::ParserUnexpectedToken, SourceLocation()) << 'a' << "xyz";

    EXPECT_EQ(DiagId::ParserUnexpectedToken, record.id);
    EXPECT_STREQ("PARSE-EXCEPTION", record.code);
    EXPECT_EQ("syntax error, unexpected a, expecting xyz", record.message);
    EXPECT_EQ(DiagLevel::Error, record.level);
}

TEST_F(DiagManagerTest, BuilderCatchesExceptions) {
    EXPECT_CALL(mockDiagProcessor, process(::testing::_)).WillOnce(::testing::Throw(std::exception()));

    EXPECT_NO_THROW(diagMgr.report(DiagId::ScannerInvalidCharacter, SourceLocation()) << 'a';);
}

TEST_F(DiagManagerTest, Disable) {
    EXPECT_CALL(mockDiagProcessor, process(::testing::_)).Times(0);

    DisableDiag dd(diagMgr);
    diagMgr.report(DiagId::ParserUnexpectedToken, SourceLocation()) << 'a' << "xyz";
}

} // namespace comp
} // namespace qore
