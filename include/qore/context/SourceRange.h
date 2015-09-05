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
/// \brief Describes a range of locations in the source code.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONTEXT_SOURCERANGE_H_
#define INCLUDE_QORE_CONTEXT_SOURCERANGE_H_

#include "qore/context/SourceLocation.h"

namespace qore {

/**
 * \brief A range of locations in the source code.
 */
struct SourceRange {

    SourceLocation start;           //!< The start of the range.
    SourceLocation end;             //!< The end of the range.

    SourceRange() = default;

    /**
     * \brief Creates a range.
     * \param start the start of the range
     * \param end the end of the range
     */
    SourceRange(SourceLocation start, SourceLocation end) : start(start), end(end) {
    }

    /**
     * \brief Compares two source ranges for equality.
     * \param other the SourceRange `this` should be compared to
     * \return true if `this` equals `other`
     */
    bool operator==(const SourceRange &other) const {
        return start == other.start && end == other.end;
    }

    /**
     * \brief Compares two source range for inequality.
     * \param other the SourceRange `this` should be compared to
     * \return true if `this` does not equal `other`
     */
    bool operator!=(const SourceRange &other) const {
        return !(*this == other);
    }
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_SOURCERANGE_H_
