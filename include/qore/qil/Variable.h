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
/// \brief Variable descriptor.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_VARIABLE_H_
#define INCLUDE_QORE_QIL_VARIABLE_H_

#include <ostream>
#include <string>
#include "qore/context/SourceLocation.h"

namespace qore {
namespace qil {

/**
 * \brief Describes a variable.
 */
class Variable {

public:
    /**
     * \brief Type of data associated with the variable.
     */
    using Data = void *;

    /**
     * \brief Constructor.
     * \param name the name of the variable
     * \param location the location of the declaration in the source
     */
    Variable(std::string name, const SourceLocation &location);
    ~Variable();

public:
    const std::string name;             //!< The name of the variable.
    const SourceLocation location;      //!< The location of declaration in the source.
    Data data;                          //!< Data associated with the variable.

private:
    Variable(const Variable &) = delete;
    Variable(Variable &&) = delete;
    Variable &operator=(const Variable &) = delete;
    Variable &operator=(Variable &&) = delete;
};

/**
 * \brief Writes information about a variable to an output stream.
 * \param os the output stream
 * \param var the variable
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const Variable &var) {
    return os << "Variable " << var.name << " @" << var.location << " (data: " << var.data << ")";
}

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_VARIABLE_H_
