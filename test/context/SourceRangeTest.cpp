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
#include <sstream>
#include "gtest/gtest.h"
#include "qore/context/SourceRange.h"
#include "SourceTestHelper.h"

namespace qore {

struct SourceRangeTest : ::testing::Test, SourceTestHelper {
};

TEST_F(SourceRangeTest, DefaultCtor) {
    SourceRange r;
    EXPECT_EQ(SourceId::Invalid, r.start.sourceId);
    EXPECT_EQ(0, r.start.line);
    EXPECT_EQ(0, r.start.column);
    EXPECT_EQ(SourceId::Invalid, r.end.sourceId);
    EXPECT_EQ(0, r.end.line);
    EXPECT_EQ(0, r.end.column);
}

TEST_F(SourceRangeTest, Ctor) {
    SourceRange r(createLocation(1, 2, 3), createLocation(4, 5, 6));
    EXPECT_EQ(createId(1), r.start.sourceId);
    EXPECT_EQ(2, r.start.line);
    EXPECT_EQ(3, r.start.column);
    EXPECT_EQ(createId(4), r.end.sourceId);
    EXPECT_EQ(5, r.end.line);
    EXPECT_EQ(6, r.end.column);
}

TEST_F(SourceRangeTest, CopyCtor) {
    SourceRange r0(createLocation(1, 2, 3), createLocation(4, 5, 6));
    SourceRange r(r0);
    EXPECT_EQ(createId(1), r.start.sourceId);
    EXPECT_EQ(2, r.start.line);
    EXPECT_EQ(3, r.start.column);
    EXPECT_EQ(createId(4), r.end.sourceId);
    EXPECT_EQ(5, r.end.line);
    EXPECT_EQ(6, r.end.column);
}

TEST_F(SourceRangeTest, CopyAssignment) {
    SourceRange r;
    EXPECT_EQ(SourceId::Invalid, r.start.sourceId);
    EXPECT_EQ(0, r.start.line);
    EXPECT_EQ(0, r.start.column);
    EXPECT_EQ(SourceId::Invalid, r.end.sourceId);
    EXPECT_EQ(0, r.end.line);
    EXPECT_EQ(0, r.end.column);

    SourceRange r0(createLocation(1, 2, 3), createLocation(4, 5, 6));
    r = r0;
    EXPECT_EQ(createId(1), r.start.sourceId);
    EXPECT_EQ(2, r.start.line);
    EXPECT_EQ(3, r.start.column);
    EXPECT_EQ(createId(4), r.end.sourceId);
    EXPECT_EQ(5, r.end.line);
    EXPECT_EQ(6, r.end.column);
}

TEST_F(SourceRangeTest, CompareEq) {
    SourceLocation loc1 = createLocation(1, 2, 3);
    SourceLocation loc2 = createLocation(4, 5, 6);
    SourceLocation loc3 = createLocation(7, 8, 9);

    SourceRange r1 = SourceRange(loc1, loc2);
    SourceRange r2 = SourceRange(loc1, loc2);
    SourceRange r3 = SourceRange(loc1, loc3);
    SourceRange r4 = SourceRange(loc3, loc2);

    EXPECT_TRUE(r1 == r2);
    EXPECT_FALSE(r1 == r3);
    EXPECT_FALSE(r1 == r4);
}

TEST_F(SourceRangeTest, CompareNe) {
    SourceLocation loc1 = createLocation(1, 2, 3);
    SourceLocation loc2 = createLocation(4, 5, 6);
    SourceLocation loc3 = createLocation(7, 8, 9);

    SourceRange r1 = SourceRange(loc1, loc2);
    SourceRange r2 = SourceRange(loc1, loc2);
    SourceRange r3 = SourceRange(loc1, loc3);
    SourceRange r4 = SourceRange(loc3, loc2);

    EXPECT_FALSE(r1 != r2);
    EXPECT_TRUE(r1 != r3);
    EXPECT_TRUE(r1 != r4);
}

TEST_F(SourceRangeTest, toStream) {
    std::ostringstream ss;
    SourceLocation loc1 = createLocation(1, 2, 3);
    SourceLocation loc2 = createLocation(4, 5, 6);
    SourceRange r1 = SourceRange(loc1, loc2);

    ss << r1;
    EXPECT_EQ("2:3-5:6", ss.str());
}

} // namespace qore
