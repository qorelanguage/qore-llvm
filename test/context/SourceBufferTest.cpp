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
#include "qore/context/SourceBuffer.h"

namespace qore {

struct SourceBufferTest : public ::testing::Test {
    std::string src{"test"};
    SourceId sourceId{4};
};

typedef SourceBufferTest SourceBufferDeathTest;

TEST_F(SourceBufferTest, StdinNewlineAndZero) {
    SourceBuffer sb(sourceId);
    EXPECT_EQ(2U, sb.data.size());
    EXPECT_EQ('\n', sb.data[0]);
    EXPECT_EQ('\0', sb.data[1]);
}

TEST_F(SourceBufferTest, CtorAddsZero) {
    SourceBuffer sb(sourceId, src.begin(), src.end());
    EXPECT_EQ(5U, sb.data.size());
    EXPECT_EQ('t', sb.data[0]);
    EXPECT_EQ('e', sb.data[1]);
    EXPECT_EQ('s', sb.data[2]);
    EXPECT_EQ('t', sb.data[3]);
    EXPECT_EQ('\0', sb.data[4]);
}

TEST_F(SourceBufferDeathTest, StdinCtorChecksSourceId) {
    EXPECT_DEATH(SourceBuffer(SourceId::Invalid), "Invalid source id");
}

TEST_F(SourceBufferDeathTest, CtorChecksSourceId) {
    EXPECT_DEATH(SourceBuffer(SourceId::Invalid, src.begin(), src.end()), "Invalid source id");
}

} // namespace qore
