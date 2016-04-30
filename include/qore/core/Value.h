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
/// \brief Definitions of types related to runtime values.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_VALUE_H_
#define INCLUDE_QORE_CORE_VALUE_H_

#include <cstdint>

namespace qore {

/**
 * \brief The type used to represent Qore 'bool' values.
 */
using qbool = bool;

/**
 * \brief The type used to represent Qore 'int' values.
 */
using qint = int64_t;

/**
 * \brief The type used to represent Qore 'float' values.
 */
using qfloat = double;

/**
 * \brief The type used to represent heap-allocated values that use reference counting.
 */
using qptr = class Any *;

/**
 * \brief A 64-bit type that can hold any value.
 */
union qvalue {
    qbool b;        //!< Holds a 'bool' value.
    qint i;         //!< Holds an 'int' value.
    qfloat f;       //!< Holds a 'float' value.
    qptr p;         //!< Holds a pointer to a heap-allocated value.
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_VALUE_H_
