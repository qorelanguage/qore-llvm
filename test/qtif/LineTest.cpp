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
    virtual bool checkDiag(qore::DiagRecord &record) {
        QTIF_REPORT << formatDiagRecord(record);
        return false;
    }

protected:
    std::string formatDiagRecord(qore::DiagRecord &record) {
        std::ostringstream s;
        s << "unexpected " << record.level << " at " << record.location << "\n"
                << "  id: " << record.id << "\n"
                << "  code: " << record.code << "\n"
                << "  message: " << record.message;
        return s.str();
    }

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
        if (!std::regex_search(str, std::regex(regex))) {
            QTIF_REPORT << "the output: " << str << "\ndoes not match: " << regex;
        }
        return true;
    }

private:
    std::string regex;
};

class DiagExpectation : public Expectation {

public:
    DiagExpectation(const std::string &fileName, int lineNumber, std::string line)
        : Expectation(fileName, lineNumber), expLine(-1), expColumn(-1) {
        std::regex regex("^([a-zA-Z]+)(@([0-9]+)(:([0-9]+))?)?(-(.*))?$");
        std::smatch m;
        if (!std::regex_match(line, m, regex)) {
            throw Exception("Invalid diagnostic expectation format: " + line, lineNumber);
        }
        assert(m.size() == 8 && m[1].matched);
        expId = m[1];
        if (m[3].matched) {
            expLine = std::stoi(m[3]);
            if (m[5].matched) {
                expColumn = std::stoi(m[5]);
            }
        }
        expMsgMatched = m[7].matched;
        expMsg = m[7];
    }

    void checkEnd() override {
        QTIF_REPORT << "expected diagnostic message:\n" << describe();
    }

    bool checkLine(const std::string &str) override {
        QTIF_REPORT << "expected diagnostic message:\n" << describe() << "\nactual output: " << str;
        return true;
    }

    bool checkDiag(qore::DiagRecord &record) override {
        bool match = true;

        std::ostringstream id;
        id << record.id;

        match &= id.str() == expId;
        if (expLine >= 0) {
            match &= expLine == record.location.line;
        }
        if (expColumn >= 0) {
            match &= expColumn == record.location.column;
        }
        if (expMsgMatched) {
            std::regex r(expMsg);
            match &= std::regex_search(record.message, r);
        }

        if (!match) {
            QTIF_REPORT << formatDiagRecord(record) << "\nexpected:\n" << describe();
        }
        return true;
    }

private:
    std::string describe() {
        std::ostringstream s;
        s << "  id: " << expId;
        if (expLine >= 0) {
            s << "\n  location: " << expLine;
            if (expColumn >= 0) {
                s << ":" << expColumn;
            }
        }
        if (expMsgMatched) {
            s << "\n  message regex: " << expMsg;
        }
        return s.str();
    }

private:
    std::string expId;
    int expLine;
    int expColumn;
    bool expMsgMatched;
    std::string expMsg;
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

LineTestOutput &LineTestOutput::operator<<(char c) {
    if (c == '\n') {
        lineTest.processOutput(buffer);
        buffer.clear();
    } else {
        buffer.push_back(c);
    }
    return *this;
}

LineTestOutput &LineTestOutput::operator<<(const std::string &str) {
    for (auto c : str) {
        *this << c;
    }
    return *this;
}

LineTestOutput &LineTestOutput::operator<<(const qore::SourceLocation &loc) {
    return *this << '@' << loc.line << ':' << loc.column;
}

LineTestOutput &LineTestOutput::operator<<(int i) {
    std::ostringstream s;
    s << i;
    return *this << s.str();
}

void LineTestOutput::flush() {
    if (!buffer.empty()) {
        lineTest.processOutput(buffer);
        buffer.clear();
    }
}

void LineTestDiagProcessor::process(qore::DiagRecord &record) {
    lineTest.processDiag(record);
}

LineTest::LineTest() : output(*this), diagProcessor(*this) {
    diagMgr.addProcessor(&diagProcessor);
}

LineTest::~LineTest() {
}

void LineTest::verify() {
    output.flush();
    (*current)->checkEnd();
}

void LineTest::processOutput(const std::string &str) {
    if ((*current)->checkLine(str)) {
        ++current;
    }
}

void LineTest::processDiag(qore::DiagRecord &record) {
    if ((*current)->checkDiag(record)) {
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
                case '!':
                    expected.emplace_back(new DiagExpectation(getFileName(), x.first, x.second.erase(0, 3)));
                    break;
                case '#':
                    //comment
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
