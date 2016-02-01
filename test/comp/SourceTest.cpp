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

TEST_F(SourceTest, wasFirstOnLine) {
    src.read();
    EXPECT_TRUE(src.wasFirstOnLine());
    src.read();
    EXPECT_FALSE(src.wasFirstOnLine());
    src.read();
    EXPECT_FALSE(src.wasFirstOnLine());
    src.read();
    EXPECT_TRUE(src.wasFirstOnLine());
    src.read();
    EXPECT_FALSE(src.wasFirstOnLine());
    src.read();
    EXPECT_FALSE(src.wasFirstOnLine());
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

TEST_F(SourceTest, decodeLocation) {
    EXPECT_EQ(std::make_pair(0, 0), src.decodeLocation(-1));
    EXPECT_EQ(std::make_pair(1, 1), src.decodeLocation(0));
    EXPECT_EQ(std::make_pair(1, 2), src.decodeLocation(1));
    EXPECT_EQ(std::make_pair(1, 3), src.decodeLocation(2));
    EXPECT_EQ(std::make_pair(2, 1), src.decodeLocation(3));
    EXPECT_EQ(std::make_pair(2, 2), src.decodeLocation(4));
    EXPECT_EQ(std::make_pair(2, 3), src.decodeLocation(5));
    EXPECT_EQ(std::make_pair(0, 0), src.decodeLocation(6));
}

TEST_F(SourceTest, decodeLocationNewLines) {
    std::string str{"a\r\nc\nd\re"};
    Source src{"abc", 123, std::vector<char>(str.begin(), str.end())};

    EXPECT_EQ(std::make_pair(1, 1), src.decodeLocation(0));
    EXPECT_EQ(std::make_pair(1, 2), src.decodeLocation(1));
    EXPECT_EQ(std::make_pair(1, 3), src.decodeLocation(2));
    EXPECT_EQ(std::make_pair(2, 1), src.decodeLocation(3));
    EXPECT_EQ(std::make_pair(2, 2), src.decodeLocation(4));
    EXPECT_EQ(std::make_pair(3, 1), src.decodeLocation(5));
    EXPECT_EQ(std::make_pair(3, 2), src.decodeLocation(6));
    EXPECT_EQ(std::make_pair(4, 1), src.decodeLocation(7));
    EXPECT_EQ(std::make_pair(4, 2), src.decodeLocation(8));
}

TEST_F(SourceTest, decodeLocationTabs) {
    std::string str{"\ta\taa\taaa\taaaa\ta"};
    Source src{"abc", 123, std::vector<char>(str.begin(), str.end())};

    EXPECT_EQ(std::make_pair(1, 1), src.decodeLocation(0));
    EXPECT_EQ(std::make_pair(1, 5), src.decodeLocation(1));
    EXPECT_EQ(std::make_pair(1, 6), src.decodeLocation(2));
    EXPECT_EQ(std::make_pair(1, 9), src.decodeLocation(3));
    EXPECT_EQ(std::make_pair(1, 10), src.decodeLocation(4));
    EXPECT_EQ(std::make_pair(1, 11), src.decodeLocation(5));
    EXPECT_EQ(std::make_pair(1, 13), src.decodeLocation(6));
    EXPECT_EQ(std::make_pair(1, 14), src.decodeLocation(7));
    EXPECT_EQ(std::make_pair(1, 15), src.decodeLocation(8));
    EXPECT_EQ(std::make_pair(1, 16), src.decodeLocation(9));
    EXPECT_EQ(std::make_pair(1, 17), src.decodeLocation(10));
    EXPECT_EQ(std::make_pair(1, 18), src.decodeLocation(11));
    EXPECT_EQ(std::make_pair(1, 19), src.decodeLocation(12));
    EXPECT_EQ(std::make_pair(1, 20), src.decodeLocation(13));
    EXPECT_EQ(std::make_pair(1, 21), src.decodeLocation(14));
    EXPECT_EQ(std::make_pair(1, 25), src.decodeLocation(15));
    EXPECT_EQ(std::make_pair(1, 26), src.decodeLocation(16));

    EXPECT_EQ(std::make_pair(1, 1), src.decodeLocation(0, 2));
    EXPECT_EQ(std::make_pair(1, 3), src.decodeLocation(1, 2));
    EXPECT_EQ(std::make_pair(1, 4), src.decodeLocation(2, 2));
    EXPECT_EQ(std::make_pair(1, 5), src.decodeLocation(3, 2));
    EXPECT_EQ(std::make_pair(1, 6), src.decodeLocation(4, 2));
    EXPECT_EQ(std::make_pair(1, 7), src.decodeLocation(5, 2));
    EXPECT_EQ(std::make_pair(1, 9), src.decodeLocation(6, 2));
}

} // namespace comp
} // namespace qore
