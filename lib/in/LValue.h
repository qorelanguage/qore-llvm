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
/// \brief LValue definition.
///
//------------------------------------------------------------------------------
#ifndef LIB_IN_LVALUE_H_
#define LIB_IN_LVALUE_H_

#include <functional>
#include "qore/ir/Type.h"
#include "qore/rt/Types.h"

namespace qore {
namespace in {

struct LValue {
    using Unlock = std::function<void()>;   //must not throw

    LValue(rt::qvalue *ptr, const ir::Type &type, Unlock unlock = nop) : ptr(ptr), type(type.rtType), unlock(unlock) {
    }

    LValue(rt::qvalue *ptr, rt::qvalue_type type, Unlock unlock = nop) : ptr(ptr), type(type), unlock(unlock) {
    }

    rt::qvalue *ptr;
    rt::qvalue_type type;
    Unlock unlock;

private:
    static void nop() noexcept {
    }
};

} // namespace in
} // namespace qore

#endif // LIB_IN_LVALUE_H_
