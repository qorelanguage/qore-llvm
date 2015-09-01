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
#ifndef TEST_CONTEXT_HELPERS_H_
#define TEST_CONTEXT_HELPERS_H_

#include <vector>
#include "qore/context/DiagManager.h"
#include "qore/context/SourceManager.h"
#include "qore/context/SourcePointer.h"

namespace qore {

struct SourceIdTestHelper {
    const SourceId sourceId1{1};
    const SourceId sourceId2{2};

    short getId(const SourceId &sourceId) const {
        return sourceId.id;
    }

    SourceId createSourceId(int value) const {
        return SourceId(value);
    }
};

struct SourceBufferTestHelper {

    SourceBuffer createSourceBuffer(SourceId sourceId) const {
        return SourceBuffer(sourceId);
    }

    SourceBuffer createSourceBuffer(SourceId sourceId, std::vector<char> data) const {
        return SourceBuffer(sourceId, std::move(data));
    }

    template<typename Iterator>
    SourceBuffer createSourceBuffer(SourceId sourceId, Iterator begin, Iterator end) const {
        return SourceBuffer(sourceId, begin, end);
    }


    SourceId getSourceId(const SourceBuffer &sb) const {
        return sb.sourceId;
    }

    bool isStdin(const SourceBuffer &sb) const {
        return sb.isStdin;
    }

    const std::vector<char> &getData(const SourceBuffer &sb) const {
        return sb.data;
    }
};

struct SourcePointerTestHelper {

    std::vector<char>::size_type getIndex(const SourcePointer &ptr) {
        return ptr.index;
    }
};

struct SourceManagerTestHelper {

    const std::vector<std::string> &getNames(const SourceManager &mgr) const {
        return mgr.names;
    }
};

struct DiagManagerTestHelper {

    const DiagRecord &extractRecord(DiagBuilder &builder) {
        return builder.record;
    }

    void callProcess(DiagManager &mgr, DiagRecord &record) {
        mgr.process(record);
    }
};

} // namespace qore

#endif // TEST_CONTEXT_HELPERS_H_
