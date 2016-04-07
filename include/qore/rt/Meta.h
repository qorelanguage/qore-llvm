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
/// \brief TODO description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_RT_META_H_
#define INCLUDE_QORE_RT_META_H_

#include "qore/rt/Types.h"

namespace qore {
namespace rt {
namespace meta {

struct ConversionDesc {
    using F = qvalue(qvalue);

    const F &f;
    const Type retType;
    const Type argType;

private:
    ConversionDesc() = delete;
};

struct BinaryOperatorDesc {
    using F = qvalue(qvalue, qvalue);

    const F &f;
    const Type retType;
    const Type leftType;
    const Type rightType;

private:
    BinaryOperatorDesc() = delete;
};

extern ConversionDesc ConversionTable[];
extern BinaryOperatorDesc BinaryOperatorTable[];

Operator findOperator(Op o, Type l, Type r);
Conversion findConversion(Type src, Type dest);

} // namespace meta
} // namespace rt
} // namespace qore

#endif // INCLUDE_QORE_RT_META_H_
