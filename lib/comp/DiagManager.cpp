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
///
/// \file
/// \brief Provides support for generating diagnostic messages.
///
//------------------------------------------------------------------------------
#include "qore/comp/DiagManager.h"
#include <string>

using std::placeholders::_1;

namespace qore {
namespace comp {

/// \cond IGNORED_BY_DOXYGEN
struct DiagInfo {
    const char *code;
    DiagLevel level;
    const char *description;
};

static const DiagInfo data[] = {
    #define DIAG(N, C, L, D)        { C, DiagLevel::L, D },
    #include "qore/comp/DiagData.inc"
    #undef DIAG
};
/// \endcond

DiagBuilder DiagManager::report(DiagId diagId, SourceLocation location) {
    const DiagInfo &info = data[static_cast<int>(diagId)];
    return DiagBuilder(std::bind(&DiagManager::process, this, _1), diagId,
            info.code, info.level, info.description, location);
}

void DiagManager::process(DiagRecord &record) {
    LOG("Processing diagnostic (disabledCounter = " << disabledCounter << "): " << record.message);
    if (disabledCounter) {
        return;
    }
    for (auto processor : processors) {
        processor->process(record);
    }
}

DiagBuilder::DiagBuilder(ProcessCallback processCallback, DiagId id, const char *code, DiagLevel level,
        std::string message, SourceLocation location)
        : processCallback(processCallback), record{id, code, level, message, location} {
}

DiagBuilder::~DiagBuilder() {
    assert(record.message.find("%s") == std::string::npos && "Missing parameter for a diagnostic message");
    try {
        processCallback(record);
    } catch (...) {
        LOG("An exception was caught during diagnostic processing");
    }
}

} // namespace comp
} // namespace qore
