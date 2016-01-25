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
#include "../Utils.h"
#include "SourceTestHelper.h"

namespace qore {

struct SourceTest : ::testing::Test, SourceTestHelper {
    std::string str{"te\nst"};
    Source src{"abc", createId(123), std::vector<char>(str.begin(), str.end()), false};
    Source stdin{"", createId(123), std::vector<char>(), true};
};

TEST_F(SourceTest, getName) {
    EXPECT_EQ("abc", src.getName());
}

#define EXPECT_CHAR_AND_LOCATION(CHAR, LINE, COLUMN, SRC)   \
    {                                                       \
        int c = CHAR;                                       \
        SourceLocation loc = SRC.getLocation();             \
        EXPECT_EQ(c, SRC.read());                           \
        EXPECT_EQ(createId(123), loc.sourceId);             \
        EXPECT_EQ(LINE, loc.line);                          \
        EXPECT_EQ(COLUMN, loc.column);                      \
    }

TEST_F(SourceTest, read) {
    EXPECT_CHAR_AND_LOCATION('t', 1, 1, src);
    EXPECT_CHAR_AND_LOCATION('e', 1, 2, src);
    EXPECT_CHAR_AND_LOCATION('\n', 1, 3, src);
    EXPECT_CHAR_AND_LOCATION('s', 2, 1, src);
    EXPECT_CHAR_AND_LOCATION('t', 2, 2, src);
    EXPECT_CHAR_AND_LOCATION(Source::EndOfFile, 2, 3, src);
    EXPECT_CHAR_AND_LOCATION(Source::EndOfFile, 2, 3, src);
}

TEST_F(SourceTest, stdin) {
    RedirectStdin redirect("x\n");
    EXPECT_CHAR_AND_LOCATION('x', 1, 1, stdin);
    EXPECT_CHAR_AND_LOCATION('\n', 1, 2, stdin);
    EXPECT_CHAR_AND_LOCATION(Source::EndOfFile, 2, 1, stdin);
    EXPECT_CHAR_AND_LOCATION(Source::EndOfFile, 2, 1, stdin);
}

#undef EXPECT_CHAR_AND_LOCATION

} // namespace qore
