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
/// \brief Qore runtime functions.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_RT_FUNC_H_
#define INCLUDE_QORE_RT_FUNC_H_

#include "qore/rt/Types.h"

namespace qore {
namespace rt {

extern "C" void incRef(qptr ptr) noexcept;
extern "C" void decRef(qptr ptr);                               //FIXME EXCEPTIONS!!!

extern "C" qptr createString(const char *str, qsize len);       //THROWS, encoding
extern "C" qptr convertIntToString(qint i);                     //THROWS
extern "C" qptr opAddStringString(qptr left, qptr right);       //THROWS

extern "C" GlobalVariable *gv_create(qvalue init);              //THROWS
extern "C" qvalue gv_free(GlobalVariable *gv) noexcept;
extern "C" qvalue gv_read_lock(GlobalVariable *gv) noexcept;
extern "C" void gv_read_unlock(GlobalVariable *gv) noexcept;
extern "C" qvalue *gv_write_lock(GlobalVariable *gv) noexcept;
extern "C" void gv_write_unlock(GlobalVariable *gv) noexcept;

extern "C" void combine(Exception &original, Exception &secondary);

extern "C" qptr int_box(qint i);
extern "C" qint int_unbox(qptr p) noexcept;

extern "C" qptr op_add_any_any(qptr left, qptr right);       //THROWS

//XXX naming conventions


//these are used internally by llvm code gen, declarations are not necessary
extern "C" qint qvalue_to_qint(qvalue v) noexcept;
extern "C" qint *qvalue_ptr_to_qint_ptr(qvalue *v) noexcept;
extern "C" qptr qvalue_to_qptr(qvalue v) noexcept;
extern "C" qptr *qvalue_ptr_to_qptr_ptr(qvalue *v) noexcept;
extern "C" qvalue qint_to_qvalue(qint i) noexcept;
extern "C" qvalue qptr_to_qvalue(qptr p) noexcept;

} // namespace rt
} // namespace qore

#endif // INCLUDE_QORE_RT_FUNC_H_
