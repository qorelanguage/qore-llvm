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
/// \brief Implementation of conversions.
///
//------------------------------------------------------------------------------
#include "Conversions.h"
#include <cassert>
#include "qore/core/Conversion.h"
#include "qore/core/Int.h"
#include "qore/core/String.h"

namespace qore {
namespace impl {

qvalue convertAny(qvalue src, const Type &type) {
    const Type &srcType = src.p ? src.p->getType() : Type::Nothing;
    const Conversion *conversion = Conversion::find(srcType, type);

    if (srcType == Type::Int) {
        src.i = static_cast<Int *>(src.p)->get();
    }
    //XXX unboxing of bool and float

    if (conversion != nullptr) {
        src = conversion->getFunction()(src);
    } else if (type.isRefCounted()) {
        src.p->incRefCount();
    }
    return src;
}

qvalue convertAnyToString(qvalue value) {
    LOG("convertAnyToString(" << value.p << ")");
    return convertAny(value, Type::String);
}

qvalue convertIntToAny(qvalue value) {
    LOG("convertIntToAny(" << value.i << ")");
    //XXX instances can be cached if Int is immutable
    qvalue result;
    result.p = new Int(value.i);
    return result;
}

qvalue convertIntToBool(qvalue value) noexcept {
    LOG("convertIntToBool(" << value.i << ")");
    qvalue result;
    result.b = value.i != 0;
    return result;
}

qvalue convertIntToString(qvalue value) {
    LOG("convertIntToString(" << value.i << ")");
    qvalue result;
    result.p = new String(std::to_string(value.i));
    return result;
}

qvalue convertStringToInt(qvalue value) {
    assert(value.p->getType() == Type::String);
    LOG("convertStringToInt(" << value.p << ")");
    qvalue result;
    result.i = static_cast<String *>(value.p)->toInt();
    return result;
}

} // namespace impl
} // namespace qore
