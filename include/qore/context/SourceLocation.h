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
/// \brief Representation of locations in the source code.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONTEXT_SOURCELOCATION_H_
#define INCLUDE_QORE_CONTEXT_SOURCELOCATION_H_

#include "qore/context/SourceId.h"

namespace qore {

/**
 * \brief Represents a location in the source code.
 */
class SourceLocation {

public:
    /**
     * \brief Constructs an empty location instance.
     *
     * The instance should be filled in later using copy assignment.
     */
    SourceLocation() : sourceId(SourceId::Invalid), column(0), line(0) {
    }

    /**
     * \brief Constructs a location with given parameters.
     * \param sourceId the source id
     * \param line the line number
     * \param column the column number
     */
    SourceLocation(SourceId sourceId, int line, int column) : sourceId(sourceId), column(column), line(line) {
    }

private:
    SourceId sourceId;
    short column;
    int line;
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_SOURCELOCATION_H_
