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
#ifndef TEST_QTIF_SIMPLETEST_H_
#define TEST_QTIF_SIMPLETEST_H_

#include "AbstractTest.h"

namespace qtif {

/**
 * \brief A basic test that handles the 'expectations' part of the input file as a string that must match
 * the actual output exactly.
 *
 * Example use:
 *
 *     class MyTestCase : public qtif::Test {};
 *     TEST_P(MyTestCase, TestName) {
 *         verify(testedFunction(getInput());
 *     }
 *     QTIF_TEST_CASE(MyTestCase, "input/file/filter")
 */
class SimpleTest : public AbstractTest {

protected:
    void parseExpectations(Reader &reader) override {
        lineNumber = reader.getLine();
        expected = reader.getRest<std::string>();
    }

    /**
     * \brief Called from a test to compare the actual output to the expectations part of the input file.
     * \param actual the actual output of the component being tested
     */
    void verify(const std::string &actual) {
        if (expected != actual) {
            QTIF_MESSAGE(lineNumber)
                    << "expected:\n---START---\n" << expected << "\n---END---\n"
                    << "actual:\n---START---\n" << actual << "\n---END---";
        }
    }

private:
    int lineNumber;
    std::string expected;
};

} // namespace qtif

#endif // TEST_QTIF_SIMPLETEST_H_
