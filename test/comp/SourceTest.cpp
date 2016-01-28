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
#include "qore/comp/Source.h"

namespace qore {
namespace comp {

struct SourceTest : ::testing::Test {
    std::string str{"ab\ncd"};
    Source src{"abc", 123, std::vector<char>(str.begin(), str.end())};
};

TEST_F(SourceTest, getName) {
    EXPECT_EQ("abc", src.getName());
}

TEST_F(SourceTest, getId) {
    EXPECT_EQ(123, src.getId());
}

TEST_F(SourceTest, read) {
    EXPECT_EQ('a', src.read());
    EXPECT_EQ('b', src.read());
}

TEST_F(SourceTest, peek) {
    EXPECT_EQ('a', src.peek());
    EXPECT_EQ('a', src.peek());
}

TEST_F(SourceTest, unread) {
    EXPECT_EQ('a', src.read());
    src.unread();
    EXPECT_EQ('a', src.read());
}

TEST_F(SourceTest, append) {
    EXPECT_EQ('a', src.read());
    EXPECT_EQ('b', src.read());
    EXPECT_EQ('\n', src.read());
    EXPECT_EQ('c', src.read());
    EXPECT_EQ('d', src.read());
    EXPECT_EQ('\0', src.peek());
    src.append("x");
    EXPECT_EQ('x', src.read());
    EXPECT_EQ('\0', src.read());
}

TEST_F(SourceTest, eof) {
    EXPECT_EQ('a', src.read());
    EXPECT_EQ('b', src.read());
    EXPECT_EQ('\n', src.read());
    EXPECT_EQ('c', src.read());
    EXPECT_EQ('d', src.read());
    EXPECT_EQ('\0', src.read());
    EXPECT_EQ('\0', src.read());
}

TEST_F(SourceTest, isFirstOnLine) {
    EXPECT_TRUE(src.isFirstOnLine());
    src.read();
    EXPECT_FALSE(src.isFirstOnLine());
    src.read();
    EXPECT_FALSE(src.isFirstOnLine());
    src.read();
    EXPECT_TRUE(src.isFirstOnLine());
    src.read();
    EXPECT_FALSE(src.isFirstOnLine());
    src.read();
    EXPECT_FALSE(src.isFirstOnLine());
    src.read();
}

TEST_F(SourceTest, getMarkLocation) {
    src.read();
    src.setMark();
    src.read();
    src.read();
    src.read();
    SourceLocation l = src.getMarkLocation();
    EXPECT_EQ(123, l.sourceId);
    EXPECT_EQ(1, l.offset);
}

TEST_F(SourceTest, getMarkedString) {
    src.read();
    src.setMark();
    src.read();
    src.read();
    src.read();
    EXPECT_EQ("b\nc", src.getMarkedString());
}

TEST_F(SourceTest, getMarkedLength) {
    src.read();
    src.setMark();
    src.read();
    src.read();
    src.read();
    EXPECT_EQ(3, src.getMarkedLength());
}

} // namespace comp
} // namespace qore
