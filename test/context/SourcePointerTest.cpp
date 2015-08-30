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
#include "qore/context/SourcePointer.h"
#include "../Utils.h"

namespace qore {

struct SourcePointerTest : public ::testing::Test {
    std::string src{"test"};
    SourceId sourceId{4};
    SourceBuffer sourceBuffer{sourceId, src.begin(), src.end()};
    SourceBuffer stdinBuffer{sourceId};

    std::vector<char>::size_type getIndex(const SourcePointer &ptr) {
        return ptr.index;
    }
};

typedef SourcePointerTest SourcePointerDeathTest;

TEST_F(SourcePointerTest, CastToCharPtr) {
    SourcePointer ptr(&sourceBuffer);
    EXPECT_STREQ("test", ptr);
}

TEST_F(SourcePointerTest, PreIncrement) {
    SourcePointer ptr(&sourceBuffer);
    EXPECT_STREQ("est", ++ptr);
    EXPECT_STREQ("est", ptr);
}

TEST_F(SourcePointerTest, PostIncrement) {
    SourcePointer ptr(&sourceBuffer);
    EXPECT_STREQ("test", ptr++);
    EXPECT_STREQ("est", ptr);
}

TEST_F(SourcePointerTest, IncrementStopsAtEnd) {
    SourcePointer ptr(&sourceBuffer);
    EXPECT_STREQ("est", ++ptr);
    EXPECT_STREQ("st", ++ptr);
    EXPECT_STREQ("t", ++ptr);
    EXPECT_STREQ("", ++ptr);
    EXPECT_EQ(4U, getIndex(ptr));
    EXPECT_STREQ("", ++ptr);
    EXPECT_EQ(4U, getIndex(ptr));
}

TEST_F(SourcePointerTest, StdinFill) {
    RedirectStdin redirect("x\n");
    SourcePointer ptr(&stdinBuffer);
    EXPECT_STREQ("\n", ptr);
    EXPECT_STREQ("x\n", ++ptr);
}

TEST_F(SourcePointerTest, StdinFillEof) {
    RedirectStdin redirect("");
    SourcePointer ptr(&stdinBuffer);
    EXPECT_STREQ("\n", ptr);
    EXPECT_STREQ("", ++ptr);
}

TEST_F(SourcePointerDeathTest, CtorCheckNullptr) {
    EXPECT_DEATH(SourcePointer(nullptr), "No buffer provided");
}

} // namespace qore
