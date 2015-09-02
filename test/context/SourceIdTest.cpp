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
#include "qore/context/SourceId.h"
#include "Helpers.h"

namespace qore {

struct SourceIdTest : ::testing::Test, SourceIdTestHelper {
};

typedef SourceIdTest SourceIdDeathTest;

TEST_F(SourceIdTest, InvalidIsNegative) {
    EXPECT_LT(getId(SourceId::Invalid), 0);
}

TEST_F(SourceIdTest, CtorSavesId) {
    EXPECT_EQ(1, getId(sourceId1));
}

TEST_F(SourceIdTest, Equals) {
    EXPECT_TRUE(sourceId1 == sourceId1);
    EXPECT_FALSE(sourceId1 == sourceId2);
}

TEST_F(SourceIdTest, NotEquals) {
    EXPECT_FALSE(sourceId1 != sourceId1);
    EXPECT_TRUE(sourceId1 != sourceId2);
}

#ifndef NDEBUG
TEST_F(SourceIdDeathTest, CtorChecksNegative) {
    EXPECT_DEATH(createSourceId(-123), "not be negative");
}

TEST_F(SourceIdDeathTest, CtorChecksOutOfRange) {
    EXPECT_DEATH(createSourceId(32768), "not be negative");
}
#endif

} // namespace qore
