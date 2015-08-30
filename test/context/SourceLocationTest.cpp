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
#include "qore/context/SourceLocation.h"
#include "Helpers.h"

namespace qore {

struct SourceLocationTest : public ::testing::Test, protected SourceIdTestHelper {
};

TEST_F(SourceLocationTest, DefaultCtor) {
    SourceLocation loc;
    EXPECT_EQ(SourceId::Invalid, loc.sourceId);
    EXPECT_EQ(0, loc.line);
    EXPECT_EQ(0, loc.column);
}

TEST_F(SourceLocationTest, Ctor) {
    SourceLocation loc(sourceId1, 123, 456);
    EXPECT_EQ(sourceId1, loc.sourceId);
    EXPECT_EQ(123, loc.line);
    EXPECT_EQ(456, loc.column);
}

TEST_F(SourceLocationTest, CopyCtor) {
    SourceLocation loc0(sourceId1, 123, 456);
    SourceLocation loc(loc0);
    EXPECT_EQ(sourceId1, loc.sourceId);
    EXPECT_EQ(123, loc.line);
    EXPECT_EQ(456, loc.column);
}

TEST_F(SourceLocationTest, CopyAssignment) {
    SourceLocation loc;
    EXPECT_EQ(SourceId::Invalid, loc.sourceId);
    EXPECT_EQ(0, loc.line);
    EXPECT_EQ(0, loc.column);

    SourceLocation loc0(sourceId1, 123, 456);
    loc = loc0;
    EXPECT_EQ(sourceId1, loc.sourceId);
    EXPECT_EQ(123, loc.line);
    EXPECT_EQ(456, loc.column);
}

} // namespace qore
