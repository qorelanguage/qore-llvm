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
#ifndef TEST_QTIF_ABSTRACTTEST_H_
#define TEST_QTIF_ABSTRACTTEST_H_

#include "gtest/gtest.h"
#include "Core.h"
#include "Reader.h"

namespace qtif {

/**
 * \brief Reports a fatal error.
 *
 * The macro is used the same way as similar gtest macros:
 *
 *     QTIF_FATAL(123) << "error message with a " << paramValue << " etc.";
 *
 * \param line the line number
 */
#define QTIF_FATAL(line) GTEST_MESSAGE_AT_(getFileName().c_str(), line, "", testing::TestPartResult::kFatalFailure)

/**
 * \brief Reports a non-fatal error.
 *
 * The macro is used the same way as similar gtest macros:
 *
 *     QTIF_MESSAGE(123) << "error message with a " << paramValue << " etc.";
 *
 * \param line the line number
 */
#define QTIF_MESSAGE(line) GTEST_MESSAGE_AT_(getFileName().c_str(), line, "", testing::TestPartResult::kNonFatalFailure)

/**
 * \brief Base class for tests.
 *
 * Reads the input file and splits it into two parts - 'input' and 'expectations' - separated by the separator `#$$$`.
 * The subclass needs to override the parseExpectations() method to parse the expectations part of the input file.
 * The actual input to the test is available by calling getInput().
 */
class AbstractTest : public testing::TestWithParam<std::string> {

public:
    void SetUp() override;
    void TearDown() override {
        if (setUpDone) {
            verify();
        }
    }

protected:
    /**
     * \brief Called during test setup to parse the expectations part of the input file.
     * \param reader the input file reader located just after the separator
     * \throws qtif::Exception on error
     */
    virtual void parseExpectations(Reader &reader) = 0;

    /**
     * \brief Called during TearDown() to check the end of the output.
     *
     * This method is not called if parseExpectation() throws an exception.
     */
    virtual void verify() {
    }

    /**
     * Returns the name of the input file.
     * \return the name of the input file
     */
    const std::string &getFileName() const {
        return fileName;
    }

    /**
     * Returns the input data, i.e. the content of the input file up to (but excluding) the separator.
     * \return the input for the test
     */
    const std::vector<char> &getInput() const {
        return input;
    }

    /**
     * Returns the input data, i.e. the content of the input file up to (but excluding) the separator.
     * \return the input for the test
     */
    std::string getInputAsString() const {
        return std::string{input.begin(), input.end()};
    }

private:
    static int findSeparator(Reader &reader);
    static std::vector<char> readFile(const std::string &fileName);

private:
    std::string fileName;
    std::vector<char> input;
    bool setUpDone;
};

} // namespace qtif

#endif // TEST_QTIF_ABSTRACTTEST_H_
