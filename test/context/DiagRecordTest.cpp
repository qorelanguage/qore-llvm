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
#include <sstream>
#include "qore/context/DiagRecord.h"
#include "qore/common/Util.h"

namespace qore {

TEST(DiagRecordTest, Ctor) {
    DiagRecord dr{DiagId::ScannerInvalidCharacter, DiagLevel::Warning, "", {SourceId::Invalid, 1, 2}};
}

TEST(DiagRecordTest, LevelToStream) {
    std::ostringstream ss;
    ss << "*" << DiagLevel::Error << "#" << DiagLevel::Warning << "$";
    EXPECT_EQ("*error#warning$", ss.str());
}

#ifdef QORE_COVERAGE
TEST(DiagRecordTest, LevelToStreamErr) {
    std::ostringstream ss;
    EXPECT_THROW(ss << static_cast<DiagLevel>(999), class Unreachable);
}
#endif

} // namespace qore
