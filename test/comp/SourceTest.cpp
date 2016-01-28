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
    std::string str{"te"};
    Source src{"abc", 123, std::vector<char>(str.begin(), str.end())};

    void expectLocation(int expectedOffset) {
        SourceLocation l = src.getLocation();
        EXPECT_EQ(123, l.sourceId);
        EXPECT_EQ(expectedOffset, l.offset);
    }
};

TEST_F(SourceTest, getName) {
    EXPECT_EQ("abc", src.getName());
}

TEST_F(SourceTest, read) {
    EXPECT_EQ('t', src.read());
    expectLocation(1);
    EXPECT_EQ('e', src.read());
}

TEST_F(SourceTest, peek) {
    EXPECT_EQ('t', src.peek());
    expectLocation(0);
    EXPECT_EQ('t', src.peek());
}

TEST_F(SourceTest, unread) {
    EXPECT_EQ('t', src.read());
    src.unread();
    expectLocation(0);
    EXPECT_EQ('t', src.read());
}

TEST_F(SourceTest, append) {
    EXPECT_EQ('t', src.read());
    EXPECT_EQ('e', src.read());
    EXPECT_EQ('\0', src.peek());
    expectLocation(2);
    src.append("x");
    expectLocation(2);
    EXPECT_EQ('x', src.read());
    expectLocation(3);
    EXPECT_EQ('\0', src.read());
}

TEST_F(SourceTest, eof) {
    EXPECT_EQ('t', src.read());
    EXPECT_EQ('e', src.read());
    expectLocation(2);
    EXPECT_EQ('\0', src.read());
    expectLocation(2);
    EXPECT_EQ('\0', src.read());
}

TEST_F(SourceTest, isFirstOnLine) {
    std::string str = "ab\ncd";
    Source s{"abc", 123, std::vector<char>(str.begin(), str.end())};
    EXPECT_TRUE(s.isFirstOnLine());
    s.read();
    EXPECT_FALSE(s.isFirstOnLine());
    s.read();
    EXPECT_FALSE(s.isFirstOnLine());
    s.read();
    EXPECT_TRUE(s.isFirstOnLine());
    s.read();
    EXPECT_FALSE(s.isFirstOnLine());
    s.read();
    EXPECT_FALSE(s.isFirstOnLine());
    s.read();
}

} // namespace comp
} // namespace qore
