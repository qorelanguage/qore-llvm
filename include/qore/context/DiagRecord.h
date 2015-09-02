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
/// \brief Defines the structure of diagnostic messages.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONTEXT_DIAGRECORD_H_
#define INCLUDE_QORE_CONTEXT_DIAGRECORD_H_

#include <ostream>
#include <string>
#include "qore/context/SourceLocation.h"

namespace qore {

/**
 * \brief Enumeration of all diagnostic messages.
 *
 * \ref DiagData.inc
 */
enum class DiagId {
    #define DIAG(N, L, D)       N
    /// \cond IGNORED_BY_DOXYGEN
    #include "qore/context/DiagData.inc"
    /// \endcond
    #undef DIAG
};

/**
 * \brief Diagnostic levels.
 */
enum class DiagLevel {
    Error,          //!< Error
    Warning         //!< Warning
};

/**
 * \brief Writes diagnostic level to an output stream.
 * \param o the output stream
 * \param level the diagnostic level to write
 * \return the output stream
 */
std::ostream &operator<<(std::ostream &o, DiagLevel level);

/**
 * \brief Represents a diagnostic message.
 */
struct DiagRecord {
    DiagId id;                      //!< Identifier of the diagnostic.
    DiagLevel level;                //!< Diagnostic level.
    std::string message;            //!< Diagnostic message.
    SourceLocation location;        //!< Location in the source.
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_DIAGRECORD_H_
