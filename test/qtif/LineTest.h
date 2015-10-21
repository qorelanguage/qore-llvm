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
#ifndef TEST_QTIF_LINETEST_H_
#define TEST_QTIF_LINETEST_H_

#include <memory>
#include "AbstractTest.h"

namespace qtif {

class LineTestOutput {
public:
    LineTestOutput(class LineTest &lineTest) : lineTest(lineTest) {
    }

    class LineTest &lineTest;
    std::string buffer;
};

/**
 * \brief A basic test that handles the 'expectations' part of the input file on a line-by-line basis.
 *
 * Each line of the output must match the corresponding expected line exactly. This provides better localization
 * of error messages compared to SimpleTest at the expense of ignoring the differences in line endings.
 * The LineOutputHelper class can be used to convert a stream of output characters into a vector of strings:
 *
 *     class MyTestCase : public qtif::LineTest {};
 *     TEST_P(MyTestCase, TestName) {
 *         TestedComponent x(getInput());
 *         while (x.hasMore()) {
 *             output << x.produceCharacter()
 *         }
 *     }
 *     QTIF_TEST_CASE(MyTestCase, "input/file/filter")
 */
class LineTest : public AbstractTest {

public:
    LineTest();
    ~LineTest();
    void TearDown() override;
    void parseExpectations(Reader &reader) override;
    void processOutput(const std::string &str);

protected:
    LineTestOutput output;

private:
    std::vector<std::unique_ptr<class Expectation>> expected;
    std::vector<std::unique_ptr<class Expectation>>::iterator current;
};

inline LineTestOutput &operator<<(LineTestOutput &output, char c) {
    if (c == '\n') {
        output.lineTest.processOutput(output.buffer);
        output.buffer.clear();
    } else {
        output.buffer.push_back(c);
    }
    return output;
}

} // namespace qtif

#endif // TEST_QTIF_LINETEST_H_
