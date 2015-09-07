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
#include <sstream>
#include "qore/scanner/Token.h"
#include "qore/common/Util.h"

namespace qore {

struct TokenTest : ::testing::Test {

    std::string toStream(const Token &t) {
        std::ostringstream ss;
        ss << t;
        return ss.str();
    }
};

TEST_F(TokenTest, TokenTypeToString) {
    EXPECT_EQ("None", to_string(TokenType::None));
    #define TOK(N)  EXPECT_EQ(#N, to_string(TokenType::N));
    #include "qore/scanner/TokenData.inc"
    #undef TOK
}

TEST_F(TokenTest, TokenTypeToStream) {
    std::ostringstream ss;
    ss << TokenType::None;
    EXPECT_EQ("None", ss.str());
}

#ifdef QORE_COVERAGE
TEST_F(TokenTest, TokenTypeToStringErr) {
    std::ostringstream ss;
    EXPECT_THROW(ss << static_cast<TokenType>(999), class Unreachable);
}
#endif

TEST_F(TokenTest, TokenToStream) {
    Token t;
    t.intValue = 123;
    t.stringValue = "dummy";

    t.type = TokenType::Plus;
    EXPECT_EQ("{Plus}", toStream(t));

    t.type = TokenType::Integer;
    EXPECT_EQ("{Integer, intValue = 123}", toStream(t));

    t.type = TokenType::String;
    EXPECT_EQ("{String, stringValue = dummy}", toStream(t));
}

TEST_F(TokenTest, TokenToString) {
    Token t;
    t.intValue = 123;
    t.stringValue = "dummy";

    t.type = TokenType::Plus;
    EXPECT_EQ("{Plus}", static_cast<std::string>(t));

    t.type = TokenType::Integer;
    EXPECT_EQ("{Integer, intValue = 123}", static_cast<std::string>(t));

    t.type = TokenType::String;
    EXPECT_EQ("{String, stringValue = dummy}", static_cast<std::string>(t));
}

} // namespace qore
