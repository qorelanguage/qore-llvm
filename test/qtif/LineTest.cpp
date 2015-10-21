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
#include "LineTest.h"
#include <regex>

namespace qtif {

#define QTIF_REPORT GTEST_MESSAGE_AT_(fileName.c_str(), lineNumber, "", testing::TestPartResult::kNonFatalFailure)

class Expectation {

public:
    Expectation(const std::string &fileName, int lineNumber) : fileName(fileName), lineNumber(lineNumber) {
    }

    virtual ~Expectation() = default;

    virtual void checkEnd() = 0;
    virtual bool checkLine(const std::string &) = 0;

private:
    Expectation(const Expectation &) = delete;
    Expectation(Expectation &&) = delete;
    Expectation &operator=(const Expectation &) = delete;
    Expectation &operator=(Expectation &&) = delete;

protected:
    const std::string &fileName;
    int lineNumber;
};

class StringExpectation : public Expectation {

public:
    StringExpectation(const std::string &fileName, int lineNumber, std::string line)
        : Expectation(fileName, lineNumber), line(std::move(line)) {
    }

    void checkEnd() override {
        QTIF_REPORT << "expected additional output: " << line;
    }

    bool checkLine(const std::string &str) override {
        if (str != line) {
            QTIF_REPORT << "expected: " << line << "\nactual: " << str;
        }
        return true;
    }

private:
    std::string line;
};

class RegexExpectation : public Expectation {

public:
    RegexExpectation(const std::string &fileName, int lineNumber, std::string regex)
        : Expectation(fileName, lineNumber), regex(std::move(regex)) {
    }

    void checkEnd() override {
        QTIF_REPORT << "expected additional output matching: " << regex;
    }

    bool checkLine(const std::string &str) override {
        std::cout << "MATCHING " << str << " to " << regex << "\n";
        if (!std::regex_search(str, std::regex(regex))) {
            QTIF_REPORT << "the output: " << str << "\ndoes not match: " << regex;
        }
        return true;
    }

private:
    std::string regex;
};

class EndSentinel : public Expectation {
public:
    EndSentinel(const std::string &fileName, int lineNumber) : Expectation(fileName, lineNumber) {
    }

    void checkEnd() override {
        //OK
    }

    bool checkLine(const std::string &str) override {
        QTIF_REPORT << "unexpected output: " << str;
        return false;
    }
};

LineTest::LineTest() : output(*this) {
}

LineTest::~LineTest() {
}

void LineTest::verify() {
    if (!output.buffer.empty()) {
        processOutput(output.buffer);
    }
    (*current)->checkEnd();
}

void LineTest::processOutput(const std::string &str) {
    if ((*current)->checkLine(str)) {
        ++current;
    }
}

void LineTest::parseExpectations(Reader &reader) {
    while (!reader.eof()) {
        auto x = reader.readLine();
        if (x.second.length() >= 3 && x.second[0] == '#' && x.second[1] == '$') {
            switch (x.second[2]) {
                case '$':
                    expected.emplace_back(new StringExpectation(getFileName(), x.first, x.second.erase(1, 1)));
                    break;
                case '~':
                    expected.emplace_back(new RegexExpectation(getFileName(), x.first, x.second.erase(0, 3)));
                    break;
                default:
                    throw Exception("Invalid expectation format: " + x.second, x.first);
            }
        } else {
            expected.emplace_back(new StringExpectation(getFileName(), x.first, x.second));
        }
    }
    expected.emplace_back(new EndSentinel(getFileName(), reader.getLineNumber()));
    current = expected.begin();
}

} // namespace qtif
