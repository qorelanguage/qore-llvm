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
#ifndef INCLUDE_QORE_CONTEXT_DIAGNOSTICMANAGER_H_
#define INCLUDE_QORE_CONTEXT_DIAGNOSTICMANAGER_H_

#include "qore/context/SourceLocation.h"

namespace qore {

/**
 * \brief Enumeration of all diagnostic messages.
 */
enum class Diagnostic {
    #define DIAG(N, L, D)       N,
    /// \cond IGNORED_BY DOXYGEN
    #include "qore/context/DiagnosticData.inc"
    /// \endcond IGNORED_BY DOXYGEN
    #undef DIAG
    DiagnosticCount
};

/**
 * \brief Diagnostic levels.
 */
enum class DiagnosticLevel {
    Error,          //!< Error
    Warning         //!< Warning
};

/**
 * \brief Manages reporting of diagnostic messages.
 */
class DiagnosticManager {

public:
    /**
     * \brief A helper class for building diagnostic messages with additional parameters.
     */
    class Builder {
    public:
        ~Builder();

    private:
        Builder(DiagnosticManager *, const class DiagInfo &, SourceLocation location);

    private:
        DiagnosticManager *mgr;
        const class DiagInfo &info;
        const SourceLocation location;
        friend class DiagnosticManager;
    };

public:
    /**
     * \brief Reports a message.
     * \param diagnostic the type of the diagnostic message
     * \param location the location in the source
     * \return a builder for providing additional info
     */
    Builder report(Diagnostic diagnostic, SourceLocation location);
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_DIAGNOSTICMANAGER_H_
