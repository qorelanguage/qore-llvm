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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#include "qore/rt/BinaryOperators.h"
#include <cassert>
#include "qore/rt/Conversions.h"
#include "qore/BinaryOperator.h"
#include "qore/String.h"
#include "qore/Type.h"

namespace qore {
namespace rt {

qvalue convertAny(qvalue src, const Type &type);

qvalue binOpGeneric(BinaryOperator::Kind kind, qvalue left, qvalue right) {
    const BinaryOperator &op = BinaryOperator::find(kind, left.p->getType(), right.p->getType());

    //FIXME exceptions are not handled correctly
    left = convertAny(left, op.getLeftType());
    right = convertAny(right, op.getRightType());
    qvalue result = op.getFunction()(left, right);
    if (op.getRightType().isRefCounted()) {
        right.p->decRefCount();
    }
    if (op.getLeftType().isRefCounted()) {
        left.p->decRefCount();
    }
    if (op.getResultType() == Type::Int) {
        result = convertIntToAny(result);
    }
    return result;
}

qvalue binOpAnyPlusAny(qvalue left, qvalue right) {
    //left or right can be nullptr
    LOG("binOpAnyPlusAny: " << left.p << " + " << right.p);
    return binOpGeneric(BinaryOperator::Kind::Plus, left, right);
}

qvalue binOpAnyPlusEqualsAny(qvalue left, qvalue right) {
    //left or right can be nullptr
    LOG("binOpAnyPlusEqualsAny: " << left.p << " + " << right.p);
    return binOpGeneric(BinaryOperator::Kind::PlusEquals, left, right);
}

qvalue binOpSoftStringPlusSoftString(qvalue left, qvalue right) {
    assert(left.p->getType() == Type::String && right.p->getType() == Type::String);
    LOG("binOpSoftStringPlusSoftString(" << left.p << ", " << right.p << ")");
    qvalue result;
    result.p = static_cast<String *>(left.p)->append(static_cast<String *>(right.p));
    return result;
}

qvalue binOpSoftIntPlusSoftInt(qvalue left, qvalue right) noexcept {
    LOG("binOpSoftIntPlusSoftInt(" << left.i << ", " << right.i << ")");
    qvalue result;
    result.i = left.i + right.i;
    return result;
}

} // namespace rt
} // namespace qore
