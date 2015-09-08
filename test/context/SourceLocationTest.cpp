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
#include "qore/context/SourceLocation.h"
#include "SourceTestHelper.h"

namespace qore {

struct SourceLocationTest : ::testing::Test, SourceTestHelper {
};

TEST_F(SourceLocationTest, DefaultCtor) {
    SourceLocation loc;
    EXPECT_EQ(SourceId::Invalid, loc.sourceId);
    EXPECT_EQ(0, loc.line);
    EXPECT_EQ(0, loc.column);
}

TEST_F(SourceLocationTest, Ctor) {
    SourceLocation loc(createId(1), 123, 456);
    EXPECT_EQ(createId(1), loc.sourceId);
    EXPECT_EQ(123, loc.line);
    EXPECT_EQ(456, loc.column);
}

TEST_F(SourceLocationTest, CopyCtor) {
    SourceLocation loc0(createId(1), 123, 456);
    SourceLocation loc(loc0);
    EXPECT_EQ(createId(1), loc.sourceId);
    EXPECT_EQ(123, loc.line);
    EXPECT_EQ(456, loc.column);
}

TEST_F(SourceLocationTest, CopyAssignment) {
    SourceLocation loc;
    EXPECT_EQ(SourceId::Invalid, loc.sourceId);
    EXPECT_EQ(0, loc.line);
    EXPECT_EQ(0, loc.column);

    SourceLocation loc0(createId(1), 123, 456);
    loc = loc0;
    EXPECT_EQ(createId(1), loc.sourceId);
    EXPECT_EQ(123, loc.line);
    EXPECT_EQ(456, loc.column);
}

TEST_F(SourceLocationTest, CompareEq) {
    SourceLocation loc1(createId(1), 12, 34);
    SourceLocation loc2(createId(1), 12, 34);
    SourceLocation loc3(createId(2), 12, 34);
    SourceLocation loc4(createId(1), 13, 34);
    SourceLocation loc5(createId(1), 12, 35);

    EXPECT_TRUE(loc1 == loc2);
    EXPECT_FALSE(loc1 == loc3);
    EXPECT_FALSE(loc1 == loc4);
    EXPECT_FALSE(loc1 == loc5);
}

TEST_F(SourceLocationTest, CompareNe) {
    SourceLocation loc1(createId(1), 12, 34);
    SourceLocation loc2(createId(1), 12, 34);
    SourceLocation loc3(createId(2), 12, 34);
    SourceLocation loc4(createId(1), 13, 34);
    SourceLocation loc5(createId(1), 12, 35);

    EXPECT_FALSE(loc1 != loc2);
    EXPECT_TRUE(loc1 != loc3);
    EXPECT_TRUE(loc1 != loc4);
    EXPECT_TRUE(loc1 != loc5);
}

TEST_F(SourceLocationTest, toStream) {
    std::ostringstream ss;
    SourceLocation loc1(createId(1), 12, 34);

    ss << loc1;
    EXPECT_EQ("12:34", ss.str());
}

} // namespace qore
