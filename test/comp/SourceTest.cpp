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
    SourceInfo info{"abc"};
    Source src{info, std::vector<char>(str.begin(), str.end())};
};

TEST_F(SourceTest, getName) {
    EXPECT_EQ(info, src.getInfo());
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
    EXPECT_EQ(info, l.getSourceInfo());
    EXPECT_EQ(1, l.getOffset());
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

static std::tuple<int, int, int> loc(Source &src) {
    src.setMark();
    src.read();
    SourceLocation l = src.getMarkLocation();
    return std::make_tuple(l.getOffset(), l.getLine(), l.getColumn());
}

TEST_F(SourceTest, decodeLocation) {
    EXPECT_EQ(std::make_tuple(0, 1, 1), loc(src));
    EXPECT_EQ(std::make_tuple(1, 1, 2), loc(src));
    EXPECT_EQ(std::make_tuple(2, 1, 3), loc(src));
    EXPECT_EQ(std::make_tuple(3, 2, 1), loc(src));
    EXPECT_EQ(std::make_tuple(4, 2, 2), loc(src));
    EXPECT_EQ(std::make_tuple(5, 2, 3), loc(src));
}

TEST_F(SourceTest, decodeLocationNewLines) {
    std::string str{"a\r\nc\nd\re"};
    Source src{info, std::vector<char>(str.begin(), str.end())};

    EXPECT_EQ(std::make_tuple(0, 1, 1), loc(src));
    EXPECT_EQ(std::make_tuple(1, 1, 2), loc(src));
    EXPECT_EQ(std::make_tuple(2, 1, 3), loc(src));
    EXPECT_EQ(std::make_tuple(3, 2, 1), loc(src));
    EXPECT_EQ(std::make_tuple(4, 2, 2), loc(src));
    EXPECT_EQ(std::make_tuple(5, 3, 1), loc(src));
    EXPECT_EQ(std::make_tuple(6, 3, 2), loc(src));
    EXPECT_EQ(std::make_tuple(7, 4, 1), loc(src));
    EXPECT_EQ(std::make_tuple(8, 4, 2), loc(src));
}

TEST_F(SourceTest, decodeLocationTabs) {
    std::string str{"\ta\taa\taaa\taaaa\ta"};
    Source src{info, std::vector<char>(str.begin(), str.end())};

    EXPECT_EQ(std::make_tuple(0, 1, 1), loc(src));
    EXPECT_EQ(std::make_tuple(1, 1, 5), loc(src));
    EXPECT_EQ(std::make_tuple(2, 1, 6), loc(src));
    EXPECT_EQ(std::make_tuple(3, 1, 9), loc(src));
    EXPECT_EQ(std::make_tuple(4, 1, 10), loc(src));
    EXPECT_EQ(std::make_tuple(5, 1, 11), loc(src));
    EXPECT_EQ(std::make_tuple(6, 1, 13), loc(src));
    EXPECT_EQ(std::make_tuple(7, 1, 14), loc(src));
    EXPECT_EQ(std::make_tuple(8, 1, 15), loc(src));
    EXPECT_EQ(std::make_tuple(9, 1, 16), loc(src));
    EXPECT_EQ(std::make_tuple(10, 1, 17), loc(src));
    EXPECT_EQ(std::make_tuple(11, 1, 18), loc(src));
    EXPECT_EQ(std::make_tuple(12, 1, 19), loc(src));
    EXPECT_EQ(std::make_tuple(13, 1, 20), loc(src));
    EXPECT_EQ(std::make_tuple(14, 1, 21), loc(src));
    EXPECT_EQ(std::make_tuple(15, 1, 25), loc(src));
    EXPECT_EQ(std::make_tuple(16, 1, 26), loc(src));

//tabSize == 2:
//    EXPECT_EQ(std::make_tuple(0, 1, 1), loc(src));
//    EXPECT_EQ(std::make_tuple(1, 1, 3), loc(src));
//    EXPECT_EQ(std::make_tuple(2, 1, 4), loc(src));
//    EXPECT_EQ(std::make_tuple(3, 1, 5), loc(src));
//    EXPECT_EQ(std::make_tuple(4, 1, 6), loc(src));
//    EXPECT_EQ(std::make_tuple(5, 1, 7), loc(src));
//    EXPECT_EQ(std::make_tuple(6, 1, 9), loc(src));
}

TEST_F(SourceTest, getRange) {
    EXPECT_EQ("", src.getRange(0, 0));
    EXPECT_EQ("", src.getRange(2, 0));
    EXPECT_EQ("", src.getRange(5, 0));
    EXPECT_EQ("a", src.getRange(0, 1));
    EXPECT_EQ("\n", src.getRange(2, 1));
    EXPECT_EQ("d", src.getRange(4, 1));
    EXPECT_EQ("ab\nc", src.getRange(0, 4));
    EXPECT_EQ("b\ncd", src.getRange(1, 4));
    EXPECT_EQ("ab\ncd", src.getRange(0, 5));
}

} // namespace comp
} // namespace qore
