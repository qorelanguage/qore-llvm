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

#include "AbstractTest.h"

namespace qtif {

/**
 * \brief A basic test that handles the 'expectations' part of the input file on a line-by-line basis.
 *
 * Each line of the output must match the corresponding expected line exactly. This provides better localization
 * of error messages compared to SimpleTest at the expense of ignoring the differences in line endings.
 * The LineOutputHelper class can be used to convert a stream of output characters into a vector of strings:
 *
 *     class MyTestCase : public qtif::LineTest {};
 *     TEST_P(MyTestCase, TestName) {
 *         LineOutputHelper output;
 *         TestedComponent x(getInput());
 *         while (x.hasMore()) {
 *             output += x.produceCharacter()
 *         }
 *         verify(output);
 *     }
 *     QTIF_TEST_CASE(MyTestCase, "input/file/filter")
 */
class LineTest : public AbstractTest {

protected:
    void parseExpectations(Reader &reader) override {
        while (!reader.eof()) {
            expected.push_back(reader.readLine());
        }
    }

    /**
     * \brief Called from a test to compare the actual output to the expectations part of the input file.
     * \param actual the actual output of the component being tested represented as a vector of lines
     */
    void verify(const std::vector<std::string> &actual) {
        auto count = std::min(expected.size(), actual.size());
        for (auto i = 0U; i < count; ++i) {
            if (expected[i].second == actual[i]) {
            } else {
                QTIF_MESSAGE(expected[i].first) << "expected: " << expected[i].second << "\nactual: " << actual[i];
            }
        }
        if (expected.size() > count) {
            QTIF_MESSAGE(expected[count].first) << "expected additional output: " << expected[count].second;
        }
        if (actual.size() > count) {
            QTIF_MESSAGE(expected.empty() ? 1: expected.back().first) << "unexpected output: " << actual[count];
        }
    }

private:
    std::vector<std::pair<int, std::string>> expected;
};

/**
 * \brief A helper class for converting a stream of characters into a vector of lines. Only the NL character is
 * treated as the end of a line.
 */
class LineOutputHelper {

public:
    LineOutputHelper &operator+=(int c) {
        if (c == '\n') {
            data.push_back(line);
            line.clear();
        } else {
            line.push_back(c);
        }
        return *this;
    }

    operator const std::vector<std::string> &() const {
        return data;
    }

private:
    std::string line;
    std::vector<std::string> data;
};

} // namespace qtif

#endif // TEST_QTIF_LINETEST_H_
