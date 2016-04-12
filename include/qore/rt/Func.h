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

extern "C" void incRef(qvalue ptr) noexcept;
extern "C" void decRef(qvalue ptr);                               //FIXME EXCEPTIONS!!!

extern "C" qvalue createString(const char *str, qsize len);       //THROWS, encoding
extern "C" qvalue convertIntToString(qvalue i);                     //THROWS
extern "C" qvalue convertStringToInt(qvalue p);                     //THROWS?

extern "C" qvalue opAddIntInt(qvalue left, qvalue right);       //THROWS
extern "C" qvalue opAddStringString(qvalue left, qvalue right);       //THROWS

extern "C" GlobalVariable *gv_create(qvalue init);              //THROWS
extern "C" qvalue gv_free(GlobalVariable *gv) noexcept;
extern "C" void gv_read_lock(GlobalVariable *gv) noexcept;
extern "C" void gv_read_unlock(GlobalVariable *gv) noexcept;
extern "C" void gv_write_lock(GlobalVariable *gv) noexcept;
extern "C" void gv_write_unlock(GlobalVariable *gv) noexcept;
extern "C" qvalue gv_get(GlobalVariable *gv) noexcept;
extern "C" void gv_set(GlobalVariable *gv, qvalue v) noexcept;

//extern "C" void combine(Exception &original, Exception &secondary);

extern "C" qvalue int_box(qvalue i);
extern "C" qvalue int_unbox(qvalue p) noexcept;

extern "C" qvalue op_add_any_any(qvalue left, qvalue right);       //THROWS
extern "C" qvalue op_addeq_any_any(qvalue left, qvalue right);       //THROWS

extern "C" qvalue qint_to_qvalue(qint i) noexcept;

extern "C" qvalue any_to_string(qvalue a);

//XXX naming conventions

} // namespace rt
} // namespace qore

#endif // INCLUDE_QORE_RT_FUNC_H_
