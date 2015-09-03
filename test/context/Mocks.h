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
#ifndef TEST_CONTEXT_MOCKS_H_
#define TEST_CONTEXT_MOCKS_H_

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "qore/context/DiagProcessor.h"

namespace qore {

class MockDiagProcessor : public DiagProcessor {
public:
    MOCK_METHOD1(process, void(DiagRecord &));
};

MATCHER_P(MatchDiagRecord, r, "") {
    return arg.id == r.id && arg.level == r.level && arg.message == r.message && arg.location == r.location;
}

MATCHER_P3(MatchDiagRecordIdAndLocation, expectedDiagId, expectedLine, expectedColumn, "") {
    return arg.id == expectedDiagId && arg.location.line == expectedLine && arg.location.column == expectedColumn;
}

} // namespace qore

#endif // TEST_CONTEXT_MOCKS_H_
