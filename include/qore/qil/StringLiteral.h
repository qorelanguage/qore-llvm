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
/// \brief String literal descriptor.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_STRINGLITERAL_H_
#define INCLUDE_QORE_QIL_STRINGLITERAL_H_

#include <ostream>
#include <string>
#include "qore/context/SourceLocation.h"

namespace qore {
namespace qil {

/**
 * \brief Describes a string literal.
 */
class StringLiteral {

public:
    /**
     * \brief Type of data associated with the string literal.
     */
    using Data = void *;

    /**
     * \brief Constructor.
     * \param value the value of the string literal
     * \param location the location in the source
     */
    StringLiteral(std::string value, const SourceLocation &location);
    ~StringLiteral();

public:
    const std::string value;            //!< The value of the string literal.
    const SourceLocation location;      //!< The location in the source.
    Data data;                          //!< Data associated with the variable.

private:
    StringLiteral(const StringLiteral &) = delete;
    StringLiteral(StringLiteral &&) = delete;
    StringLiteral &operator=(const StringLiteral &) = delete;
    StringLiteral &operator=(StringLiteral &&) = delete;
};

/**
 * \brief Writes information about a string literal to an output stream.
 * \param os the output stream
 * \param str the string literal
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const StringLiteral &str) {
    return os << "\"" << str.value << "\"@" << str.location;
}

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_STRINGLITERAL_H_
