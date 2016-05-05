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
/// \brief Implementation of binary operators.
///
//------------------------------------------------------------------------------
#include "BinaryOperators.h"
#include <cassert>
#include "Conversions.h"
#include "qore/core/BinaryOperator.h"
#include "qore/core/String.h"
#include "qore/core/Type.h"

namespace qore {
namespace impl {

/**
 * \brief Performs a binary operation determined at runtime based on the actual types of operands.
 * \param kind the kind of the binary operator to perform
 * \param l the value of the left operand, must be reference counted (the `p` member is active) but may be `nullptr`
 * \param r the value of the right operand, must be reference counted (the `p` member is active) but may be `nullptr`
 * \return the result of the operation with reference count increased (it is always reference counted)
 * \throws Exception if the specified kind of binary operator does not apply for the types of the values
 */
static qvalue binOpGeneric(BinaryOperator::Kind kind, qvalue l, qvalue r) {
    const BinaryOperator &op = BinaryOperator::find(kind,
            l.p ? l.p->getType() : Type::Nothing, r.p ? r.p->getType() : Type::Nothing);

    auto_ptr<qvalue> result;
    {
        auto_ptr<qvalue> left(convertAny(l, op.getLeftType()), op.getLeftType().isRefCounted());
        {
            auto_ptr<qvalue> right(convertAny(r, op.getRightType()), op.getRightType().isRefCounted());
            result = auto_ptr<qvalue>(op.getFunction()(*left, *right), op.getResultType().isRefCounted());
        }
    }
    if (op.getResultType() == Type::Int) {
        return convertIntToAny(*result);
    }
    //XXX boxing of float and bool
    return result.release();
}

qvalue binOpAnyPlusAny(qvalue left, qvalue right) {
    LOG("binOpAnyPlusAny: " << left.p << " + " << right.p);
    return binOpGeneric(BinaryOperator::Kind::Plus, left, right);
}

qvalue binOpAnyPlusEqualsAny(qvalue left, qvalue right) {
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

} // namespace impl
} // namespace qore
