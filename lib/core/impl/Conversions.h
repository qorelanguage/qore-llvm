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
/// \brief Declares functions that implement conversions.
//------------------------------------------------------------------------------
#ifndef LIB_CORE_IMPL_CONVERSIONS_H_
#define LIB_CORE_IMPL_CONVERSIONS_H_

#include "qore/core/Value.h"
#include "qore/core/Type.h"

namespace qore {
namespace impl {

///\name Conversion functions
///\{
/**
 * \brief Converts a value from one type to another.
 *
 *  Conventions:
 *    - the name is `convert<ArgType>To<ResultType>`, where `<ArgType>` is the name of the argument's type
 *      and `<ResultType>` is the name of the return value's type
 *    - if `<ArgType>` is Type::Any, then the conversion is implemented by \ref convertAny() which determines the
 *      appropriate conversion at runtime and performs it
 *    - the argument cannot be `nullptr` if `<ArgType>` is not Type::Any
 *    - if `<ResultType>` is Type::Any, then `<ArgType>` is primitive (i.e. not reference counted) and
 *      the conversion is also known as 'boxing'
 *    - the function does not decrease the reference count of its argument
 *    - the return value has its reference count increased (if `<ResultType>` is a reference counted type)
 * \param value the value to convert
 * \return the result of the conversion
 * \todo identity conversions cause increased reference count
 */
extern "C" qvalue convertAnyToString(qvalue value);
extern "C" qvalue convertIntToAny(qvalue value);
extern "C" qvalue convertIntToBool(qvalue value) noexcept;
extern "C" qvalue convertIntToString(qvalue value);
extern "C" qvalue convertStringToInt(qvalue value);
///\}

/**
 * \brief Converts a runtime value declared in the script as `any` to the specified type.
 * \param src the value to convert, must be reference counted (the `p` member is active) but may be `nullptr`
 * \param type the destination type
 * \return a value of type `type` with reference count increased (if `type` is reference counted)
 * \throws Exception if no conversion exists
 */
qvalue convertAny(qvalue src, const Type &type);

} // namespace impl
} // namespace qore

#endif // LIB_CORE_IMPL_CONVERSIONS_H_
