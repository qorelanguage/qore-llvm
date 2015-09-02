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
#include "Helpers.h"

namespace qore {

struct SourceBufferTest : ::testing::Test, SourceIdTestHelper, SourceBufferTestHelper {
    std::string src{"test"};
    std::vector<char> vec{'a', 'b', 'c'};
};

typedef SourceBufferTest SourceBufferDeathTest;

TEST_F(SourceBufferTest, StdinNewlineAndZero) {
    SourceBuffer sb = createSourceBuffer(sourceId1);
    EXPECT_EQ(2U, getData(sb).size());
    EXPECT_EQ('\n', getData(sb)[0]);
    EXPECT_EQ('\0', getData(sb)[1]);
}

TEST_F(SourceBufferTest, VectorCtorAddsZero) {
    SourceBuffer sb = createSourceBuffer(sourceId1, vec);
    EXPECT_EQ(4U, getData(sb).size());
    EXPECT_EQ('a', getData(sb)[0]);
    EXPECT_EQ('b', getData(sb)[1]);
    EXPECT_EQ('c', getData(sb)[2]);
    EXPECT_EQ('\0', getData(sb)[3]);
}

TEST_F(SourceBufferTest, IteratorCtorAddsZero) {
    SourceBuffer sb = createSourceBuffer(sourceId1, src.begin(), src.end());
    EXPECT_EQ(5U, getData(sb).size());
    EXPECT_EQ('t', getData(sb)[0]);
    EXPECT_EQ('e', getData(sb)[1]);
    EXPECT_EQ('s', getData(sb)[2]);
    EXPECT_EQ('t', getData(sb)[3]);
    EXPECT_EQ('\0', getData(sb)[4]);
}

TEST_F(SourceBufferTest, MoveCtor) {
    SourceBuffer sb = createSourceBuffer(sourceId1, src.begin(), src.end());

    EXPECT_EQ(5U, getData(sb).size());

    SourceBuffer sb2{std::move(sb)};

    EXPECT_EQ(0U, getData(sb).size());
    EXPECT_EQ(5U, getData(sb2).size());
}

TEST_F(SourceBufferTest, MoveAssign) {
    SourceBuffer sb = createSourceBuffer(sourceId1, src.begin(), src.end());
    SourceBuffer sb2 = createSourceBuffer(sourceId2, vec);

    EXPECT_EQ(5U, getData(sb).size());
    EXPECT_EQ(4U, getData(sb2).size());

    sb2 = std::move(sb);

    EXPECT_EQ(0U, getData(sb).size());
    EXPECT_EQ(5U, getData(sb2).size());
}

#ifndef NDEBUG
TEST_F(SourceBufferDeathTest, StdinCtorChecksSourceId) {
    EXPECT_DEATH(createSourceBuffer(SourceId::Invalid), "Invalid source id");
}

TEST_F(SourceBufferDeathTest, VectorCtorChecksSourceId) {
    EXPECT_DEATH(createSourceBuffer(SourceId::Invalid, vec), "Invalid source id");
}

TEST_F(SourceBufferDeathTest, CtorChecksSourceId) {
    EXPECT_DEATH(createSourceBuffer(SourceId::Invalid, src.begin(), src.end()), "Invalid source id");
}
#endif

} // namespace qore
