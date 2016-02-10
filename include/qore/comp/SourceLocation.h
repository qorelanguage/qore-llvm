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
/// \brief Representation of source locations in Qore scripts.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SOURCELOCATION_H_
#define INCLUDE_QORE_COMP_SOURCELOCATION_H_

namespace qore {
namespace comp {

/**
 * \brief Represents a location in the source code.
 */
struct SourceLocation {

public:
    int sourceId;           //!< Id of the source.
    int offset;             //!< Offset in the source.

public:
    /**
     * \brief Constructs an empty location instance.
     *
     * The instance should be filled in later using copy assignment.
     */
    SourceLocation() : sourceId(-1), offset(-1) {
    }

    /**
     * \brief Constructs a location with given parameters.
     * \param sourceId the source id
     * \param offset the offset in the source
     */
    SourceLocation(int sourceId, int offset) : sourceId(sourceId), offset(offset) {
    }
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SOURCELOCATION_H_