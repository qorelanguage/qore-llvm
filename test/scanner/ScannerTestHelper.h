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
#ifndef TEST_SCANNER_SCANNERTESTHELPER_H_
#define TEST_SCANNER_SCANNERTESTHELPER_H_

#include <queue>
#include "gmock/gmock.h"
#include "qore/scanner/Scanner.h"
#include "../context/SourceTestHelper.h"

namespace qore {

class MockScanner : public Scanner {
public:
    MOCK_METHOD1(read, void(Token *));
};

class ScannerTestHelper {

public:
    void addToken(TokenType tokenType, SourceLocation locationStart, SourceLocation locationEnd,
            uint64_t intValue, std::string stringValue) {
        Token t;
        t.type = tokenType;
        t.locationStart = locationStart;
        t.locationEnd = locationEnd;
        t.intValue = intValue;
        t.stringValue = stringValue;
        tokens.push(t);
        EXPECT_CALL(scanner, read(::testing::_))
            .Times(tokens.size())
            .WillRepeatedly(::testing::Invoke(this, &ScannerTestHelper::pop));
    }

    void addToken(TokenType tokenType,
            SourceLocation start = SourceTestHelper::createLocation(12, 34),
            SourceLocation end = SourceTestHelper::createLocation(56, 78)) {
        addToken(tokenType, start, end, 0, "");
    }

    void addToken(TokenType tokenType, uint64_t value,
            SourceLocation start = SourceTestHelper::createLocation(12, 34),
            SourceLocation end = SourceTestHelper::createLocation(56, 78)) {
        addToken(tokenType, start, end, value, "");
    }

    void addToken(TokenType tokenType, std::string value,
            SourceLocation start = SourceTestHelper::createLocation(12, 34),
            SourceLocation end = SourceTestHelper::createLocation(56, 78)) {
        addToken(tokenType, start, end, 0, value);
    }

public:
    MockScanner scanner;

private:
    void pop(Token *t) {
        *t = tokens.front();
        tokens.pop();
    }

    std::queue<Token> tokens;
};

} // namespace qore

#endif // TEST_SCANNER_SCANNERTESTHELPER_H_
