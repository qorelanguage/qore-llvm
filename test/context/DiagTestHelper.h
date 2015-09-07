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
#ifndef TEST_CONTEXT_DIAGTESTHELPER_H_
#define TEST_CONTEXT_DIAGTESTHELPER_H_

#include "gmock/gmock.h"
#include "qore/context/DiagManager.h"
#include "qore/context/DiagProcessor.h"

#define DIAG_NONE()   EXPECT_CALL(mockDiagProcessor, process(::testing::_)).Times(0)

#define DIAG_EXPECT(id, line, column)                                                       \
    EXPECT_CALL(mockDiagProcessor, process(MatchDiagRecordIdAndLocation(id, line, column))) \
        .Times(1)

#define DIAG_EXPECT_AND_CAPTURE(dest, id, line, column)                                     \
    DiagRecord dest;                                                                        \
    EXPECT_CALL(mockDiagProcessor, process(MatchDiagRecordIdAndLocation(id, line, column))) \
        .WillOnce(::testing::SaveArg<0>(&dest))

MATCHER_P(MatchDiagRecord, r, "") {
    return arg.id == r.id && arg.level == r.level && arg.message == r.message && arg.location == r.location;
}

MATCHER_P3(MatchDiagRecordIdAndLocation, expectedDiagId, expectedLine, expectedColumn, "") {
    return arg.id == expectedDiagId && arg.location.line == expectedLine && arg.location.column == expectedColumn;
}

namespace qore {

inline std::ostream &operator<<(std::ostream &os, const DiagRecord &record) {
    return os << record.id << " @ " << record.location.line << ":" << record.location.column;
}

class MockDiagProcessor : public DiagProcessor {
public:
    MOCK_METHOD1(process, void(DiagRecord &));
};

class DiagTestHelper {

public:
    DiagTestHelper() {
        diagMgr.addProcessor(&mockDiagProcessor);
    }

    DiagManager diagMgr;
    MockDiagProcessor mockDiagProcessor;
};

} // namespace qore

#endif // TEST_CONTEXT_DIAGTESTHELPER_H_
