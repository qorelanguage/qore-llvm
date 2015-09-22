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
/// \brief QoreInterLanguage namespace declaration.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_QIL_H_
#define INCLUDE_QORE_QIL_QIL_H_

namespace qore {

/**
 * \brief QoreInterLanguage representation of the code.
 *
 * QoreInterLanguage (QIL) is the code of a hypothetical stack machine described below. For the description
 * of individual instructions, see \ref Opcode.
 *
 * QIL Stack Machine
 * -----------------
 * The stack machine consists of:
 * - an operand stack,
 * - a register called `LVALREG`,
 * - a discard queue.
 *
 * Currently QIL models only the structure of basic blocks, i.e. there are no jumps or branching involved.
 *
 * The `LVALREG` represents an l-value. It is empty during 'normal' expression evaluation. During evaluation
 * of assignment expressions (even implied, such as `trim s`) it points to a memory location being written to.
 * The content of the register also represents any locks necessary to protect the value - as long as the register
 * is not empty, no value can be dereferenced.
 *
 * The discard queue is used for temporary storage of values that needs to be dereferenced once the locks held
 * by the l-value are released. In the description of the instructions (see \ref Opcode), the meaning of 'discard X'
 * is as follows:
 * > If `LVALREG` is empty, decrease the reference count of X, otherwise add X to the discard queue.
 */
namespace qil {

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_QIL_H_
