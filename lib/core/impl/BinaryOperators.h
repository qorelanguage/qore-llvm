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
/// \brief Declares functions that implement binary operators.
///
//------------------------------------------------------------------------------
#ifndef LIB_CORE_IMPL_BINARYOPERATORS_H_
#define LIB_CORE_IMPL_BINARYOPERATORS_H_

#include "qore/core/Value.h"

namespace qore {
namespace impl {

///\name Binary operators
///\{
/**
 * \brief Performs a binary operation.
 *
 *  Conventions:
 *    - the name is `binOp<LeftType><Kind><RightType>`, where `<LeftType>` is the type of the first argument,
 *      `<RightType>` is the name of the second argument and `<Kind>` is the \ref BinaryOperator::Kind of the operator
 *    - if `<LeftType>` or `<RightType>` is any of the `SoftXyz` types, then the value is already
 *      converted to type `Xyz`
 *    - if both `<LeftType>` and `<RightType>` are Type::Any, then the operation is implemented by \ref binOpGeneric()
 *      which determines the appropriate operation at runtime and performs it
 *    - an argument cannot be `nullptr` if the corresponding type is not Type::Any
 *    - the function does not decrease the reference count of its arguments
 *    - the return value has its reference count increased (if the resulting type is reference counted)
 * \param left the value of the left operand
 * \param right the value of the right operand
 * \return the result of the binary operation
 */
extern "C" qvalue binOpAnyPlusAny(qvalue left, qvalue right);
extern "C" qvalue binOpAnyPlusEqualsAny(qvalue left, qvalue right);
extern "C" qvalue binOpSoftStringPlusSoftString(qvalue left, qvalue right);
extern "C" qvalue binOpSoftIntPlusSoftInt(qvalue left, qvalue right) noexcept;
///\}

} // namespace impl
} // namespace qore

#endif // LIB_CORE_IMPL_BINARYOPERATORS_H_
