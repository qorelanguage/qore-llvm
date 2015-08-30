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

namespace qore {

struct SourceIdTest : public ::testing::Test {
    SourceId srcId123{123};
    SourceId srcId456{456};

    short getId(const SourceId &sourceId) {
        return sourceId.id;
    }
};

TEST_F(SourceIdTest, InvalidIsNegative) {
    EXPECT_LT(getId(SourceId::Invalid), 0);
}

TEST_F(SourceIdTest, CtorSavesId) {
    EXPECT_EQ(123, getId(srcId123));
}

TEST_F(SourceIdTest, Equals) {
    EXPECT_TRUE(srcId123 == srcId123);
    EXPECT_FALSE(srcId123 == srcId456);
}

TEST_F(SourceIdTest, NotEquals) {
    EXPECT_FALSE(srcId123 != srcId123);
    EXPECT_TRUE(srcId123 != srcId456);
}

TEST(SourceIdDeathTest, CtorChecksNegative) {
    EXPECT_DEATH(SourceId(-123), "not be negative");
}

TEST(SourceIdDeathTest, CtorChecksOutOfRange) {
    EXPECT_DEATH(SourceId(32768), "not be negative");
}

} // namespace qore
