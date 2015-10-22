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
#include "gmock/gmock.h"
#include "../Qtif.h"

#define QTIF(a)
#include "test_files.inc"
#undef QTIF

namespace qtif {

//------------------------------------------------------------------------------
//dummy function being tested
//------------------------------------------------------------------------------
std::string toLower(const std::string &s) {
    std::string output;
    for (auto c : s) {
        output += tolower(c);
    }
    return output;
}

//------------------------------------------------------------------------------
//qtif core
//------------------------------------------------------------------------------
TEST(QtifCore, findFiles) {
    std::vector<std::string> files = findFiles("qtif.[a-z]+1.qtif$");
    EXPECT_EQ(1U, files.size());
    EXPECT_EQ(std::string(TEST_INPUT_DIR) + "/qtif/test1.qtif", files[0]);
}

//------------------------------------------------------------------------------
//AbstractTest
//------------------------------------------------------------------------------
class QtifAbstractTest : public AbstractTest {

protected:
    void parseExpectations(Reader &reader) override {
        line = reader.getLineNumber();
        expected = reader.getRest<std::string>();
    }

protected:
    int line;
    std::string expected;
};

TEST_P(QtifAbstractTest, X) {
    EXPECT_EQ("AbC\ndEf\n", getInputAsString());
    EXPECT_EQ(4, line);
    EXPECT_EQ("abc\ndef\n", expected);
}

QTIF_TEST_CASE(QtifAbstractTest, "qtif/test1");

//------------------------------------------------------------------------------
//SimpleTest
//------------------------------------------------------------------------------
class QtifSimpleTest : public SimpleTest {
};

TEST_P(QtifSimpleTest, X) {
    verify(toLower(getInputAsString()));
}

QTIF_TEST_CASE(QtifSimpleTest, "qtif/test1");

//------------------------------------------------------------------------------
//LineTest
//------------------------------------------------------------------------------
class QtifLineTest : public LineTest {
};

TEST_P(QtifLineTest, X) {
    for (auto c : getInput()) {
        if (c == '_') {
            c = ' ';
            diagMgr.report(qore::DiagId::ScannerInvalidCharacter, qore::SourceLocation{}) << "underscore";
        }
        output << tolower(c);
    }
}

QTIF_TEST_CASE(QtifLineTest, "qtif/test2");

} // namespace qtif
