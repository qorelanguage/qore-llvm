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
#include "qore/context/DiagnosticManager.h"
#include "qore/common/Logging.h"

namespace qore {

/// \cond IGNORED_BY DOXYGEN
struct DiagInfo {
    qore::DiagnosticLevel level;
    const char *description;
};

static const DiagInfo data[] = {
    #define DIAG(N, L, D)       { qore::DiagnosticLevel::L, D },
    #include "qore/context/DiagnosticData.inc"
    #undef DIAG
};
/// \endcond IGNORED_BY DOXYGEN

DiagnosticManager::Builder DiagnosticManager::report(Diagnostic diagnostic, SourceLocation location) {
    return Builder(this, data[static_cast<int>(diagnostic)], location);
}

DiagnosticManager::Builder::Builder(DiagnosticManager *mgr, const DiagInfo &info, SourceLocation location)
: mgr(mgr), info(info), location(location) {
}

DiagnosticManager::Builder::~Builder() {
    LOG("DIAGNOSTIC: " << info.description << " here: " << location.line << ":" << location.column);
}

} // namespace qore
