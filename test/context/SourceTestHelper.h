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
#ifndef TEST_CONTEXT_SOURCETESTHELPER_H_
#define TEST_CONTEXT_SOURCETESTHELPER_H_

#include <string>
#include <vector>
#include "qore/context/SourceId.h"
#include "qore/context/SourceLocation.h"
#include "qore/context/SourceManager.h"
#include "qore/context/SourceRange.h"

namespace qore {

class SourceTestHelper {

public:
    static SourceId createId(int value) {
        return SourceId(value);
    }

    static short getId(const SourceId &sourceId) {
        return sourceId.id;
    }

    static SourceLocation createLocation(int sourceId, int line, int column) {
        return SourceLocation(createId(sourceId), line, column);
    }

    static SourceLocation createLocation(int line, int column, SourceId srcId = createId(1)) {
        return SourceLocation(srcId, line, column);
    }

    static SourceRange createRange(int line, int column1, int column2, SourceId srcId = createId(1)) {
        return SourceRange(createLocation(line, column1, srcId), createLocation(line, column2, srcId));
    }
};

} // namespace qore

#endif // TEST_CONTEXT_SOURCETESTHELPER_H_
