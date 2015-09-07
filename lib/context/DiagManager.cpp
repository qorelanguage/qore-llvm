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
#include "qore/context/DiagManager.h"
#include "qore/common/Logging.h"
#include "qore/common/Util.h"

using namespace std::placeholders;

namespace qore {

/// \cond IGNORED_BY_DOXYGEN
struct DiagInfo {
    DiagLevel level;
    const char *description;
};

static const DiagInfo data[] = {
    #define DIAG(N, L, D)       { DiagLevel::L, D },
    #include "qore/context/DiagData.inc"
    #undef DIAG
};
/// \endcond

std::ostream &operator<<(std::ostream &o, DiagLevel level) {
    switch (level) {
        case DiagLevel::Error:
            return o << "error";
        case DiagLevel::Warning:
            return o << "warning";
    }
    QORE_UNREACHABLE("Invalid value of DiagLevel: " << static_cast<int>(level));
}

std::ostream &operator<<(std::ostream &o, DiagId id) {
    switch (id) {
        #define DIAG(N, L, D)       case DiagId::N: return o << #N;
        /// \cond IGNORED_BY_DOXYGEN
        #include "qore/context/DiagData.inc"
        /// \endcond
        #undef DIAG
    }
    QORE_UNREACHABLE("Invalid value of DiagId: " << static_cast<int>(id));
}

DiagBuilder DiagManager::report(DiagId diagId, SourceLocation location) {
    const DiagInfo &info = data[static_cast<int>(diagId)];
    return DiagBuilder(std::bind(&DiagManager::process, this, _1), diagId, info.level, info.description, location);
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

DiagBuilder::DiagBuilder(ProcessCallback processCallback, DiagId id, DiagLevel level, std::string message,
        SourceLocation location)
: processCallback(processCallback), record{id, level, message, location} {
}

DiagBuilder::~DiagBuilder() {
    assert(record.message.find("%s") == std::string::npos && "Missing parameter for a diagnostic message");
    try {
        processCallback(record);
    } catch (...) {
        LOG("An exception was caught during diagnostic processing");
    }
}

} // namespace qore
