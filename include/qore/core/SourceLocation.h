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
#ifndef INCLUDE_QORE_CORE_SOURCELOCATION_H_
#define INCLUDE_QORE_CORE_SOURCELOCATION_H_

#include <iostream>
#include <string>
#include "qore/core/SourceInfo.h"

namespace qore {

/**
 * \brief Represents a location in the source code.
 */
class SourceLocation {

public:
    /**
     * \brief Creates an invalid location.
     */
    SourceLocation() : source(&SourceInfo::Invalid), lineAndColumn(0) {
    }

    /**
     * \brief Constructor.
     * \param source the information about the source file
     * \param line the line number
     * \param column the column number
     */
    SourceLocation(const SourceInfo &source, int line, int column) : source(&source) {
        if (column > 255) {
            column = 255;
        }
        lineAndColumn = line << 8 | column;
    }

    /**
     * \brief Default copy constructor.
     * \param src the source instance
     */
    SourceLocation(const SourceLocation &src) = default;

    /**
     * \brief Default copy assignment.
     * \param src the source instance
     * \return this instance
     */
    SourceLocation &operator=(const SourceLocation &src) = default;

    /**
     * \brief Returns the information about the source file.
     * \return the information about the source file
     */
    const SourceInfo &getSourceInfo() const {
        return *source;
    }

    /**
     * \brief Returns the line number.
     * \return the line number
     */
    int getLine() const {
        return lineAndColumn >> 8;
    }

    /**
     * \brief Returns the column number.
     * \return the column number
     */
    int getColumn() const {
        return lineAndColumn & 255;
    }

private:
    const SourceInfo *source;
    int lineAndColumn;
};

/**
 * \brief Writes the source location into an output stream in the form 'line:column'.
 * \param os the output stream
 * \param location the source location
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const SourceLocation &location) {
    return os << location.getLine() << ':' << location.getColumn();
}

} // namespace qore

#endif // INCLUDE_QORE_CORE_SOURCELOCATION_H_
