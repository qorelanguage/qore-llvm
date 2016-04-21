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
/// \brief Core types of the Qore runtime.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_RT_TYPES_H_
#define INCLUDE_QORE_RT_TYPES_H_

#include <cstdint>

namespace qore {
namespace rt {

//base class for all heap-allocated values (string, list, hash, user objects, primitive wrappers, etc.)
class QAny;

using qsize = uint64_t;
using qbool = bool;
using qint = int64_t;
using qfloat = double;
using qptr = QAny *;
union qvalue {
    qbool b;
    qint i;
    qfloat f;
    qptr p;
};

//struct GlobalVariable;

struct Exception {
};

enum class Type {
    Any,
    Nothing,
    Bool,
    SoftBool,
    Int,
    SoftInt,
    Float,
    Number,
    String,
    SoftString,
    List,
    Hash,
    Object,

    Error,  //should this be here?
};

enum class Op {
    Plus,
    PlusEq,
};

enum class Operator {
    IntPlusInt,
    StringPlusString,
    AnyPlusAny,
    AnyPlusEqAny,
};

enum class Conversion {
    Identity,
    IntToString,
    StringToInt,
    BoxInt,
    AnyToString,
    IntToBool,
};

} // namespace rt
} // namespace qore

#endif // INCLUDE_QORE_RT_TYPES_H_
