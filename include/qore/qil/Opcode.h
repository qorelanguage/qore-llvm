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
/// \brief Defines opcodes for the QIL.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_OPCODE_H_
#define INCLUDE_QORE_QIL_OPCODE_H_

namespace qore {
namespace qil {

/**
 * \brief QIL opcodes.
 */
enum class Opcode {

    /**
     * \brief Calculates the sum of two values.
     *
     * ### Format
     *
     *     ADD
     *
     * ### Operation
     * - pop R
     * - pop L
     * - evaluate V = L + R
     * - push V
     * - discard R
     * - discard L
     *
     * ### Stack
     *
     *     (..., L, R) -> (..., L + R)
     *
     * ### Registers
     * No change.
     */
    Add,

    /**
     * \brief Stores the value on the top of the stack to the memory location pointed to by LVALREG.
     *
     * ### Format
     *
     *     ASSIGN
     *
     * ### Operation
     * - pop NV
     * - let OV = the value of the memory location pointed to by LVALREG
     * - add OV to the discard queue
     * - store NV to the memory location pointed to by LVALREG
     * - clear LVALREG releasing the lock if necessary
     * - decrease the reference count of all values on the discard queue
     * - empty the discard queue
     *
     * ### Stack
     *
     *     (..., NV) -> (...)
     *
     * ### Registers
     * Clears `LVALREG` (must not be empty).
     */
    Assign,

    /**
     * \brief Discard the value on the top of the stack.
     *
     * ### Format
     *
     *     DISCARD
     *
     * ### Operation
     * - pop V
     * - discard V (i.e. decrease reference count of V if `LVALREG` is empty, otherwise add V to the discard queue)
     *
     * ### Stack
     *
     *     (..., V) -> (...)
     *
     * ### Registers
     * No change.
     */
    Discard,

    /**
     * \brief Duplicates the value on the top of the stack.
     *
     * ### Format
     *
     *     DUP
     *
     * ### Operation
     * - let V = the value on the top of the stack
     * - increase reference count of V
     * - push V
     *
     * ### Stack
     *
     *     (..., V) -> (..., V, V)
     *
     * ### Registers
     * No change.
     */
    Dup,

    /**
     * \brief Indicates the end of the lifetime of a variable.
     *
     * ### Format
     *
     *     LIFETIME_END <var>
     *
     * ### Operation
     * - let V = the value of \<var\>
     * - discard V
     * - deallocate space of \<var\>
     *
     * ### Stack
     * No change.
     *
     * ### Registers
     * No change.
     */
    LifetimeEnd,

    /**
     * \brief Indicates the start of the lifetime of a variable.
     *
     * ### Format
     *
     *     LIFETIME_START <var>
     *
     * ### Operation
     * - allocate space for \<var\> and set its value to `Nothing`
     *
     * ### Stack
     * No change.
     *
     * ### Registers
     * No change.
     */
    LifetimeStart,

    /**
     * \brief Loads the value pointed to by `LVALREG` cloning it if necessary.
     *
     * ### Format
     *
     *     LOAD_UNIQUE
     *
     * ### Operation
     * - let V = the value of the memory location pointed to by LVALREG
     * - if the reference count of V is 1, set it to 2 and push V
     * - otherwise, push a copy of V (with reference count set to 1)
     * - todo what if it is an object?
     *
     * ### Stack
     *
     *     (...) -> (..., V)
     *
     * ### Registers
     * `LVALREG` must not be empty.
     */
    LoadUnique,

    /**
     * \brief Loads the pointer to a variable into the `LVALREG` register.
     *
     * ### Format
     *
     *     LOAD_VAR_PTR <var>
     *
     * ### Operation
     * - set `LVALREG` = pointer to the value of \<var\>
     * - todo there will be locking involved unless var is local and not bound to any closure
     *
     * ### Stack
     * No change.
     *
     * ### Registers
     * Sets `LVALREG` (must be empty) to point to \<var\>.
     */
    LoadVarPtr,

    /**
     * \brief Writes the value on the top of the stack to the standard output.
     *
     * ### Format
     *
     *     PRINT
     *
     * ### Operation
     * - pop V
     * - write V to stdout
     * - discard V
     *
     * ### Stack
     *
     *     (..., V) -> (...)
     *
     * ### Registers
     * No change.
     */
    Print,

    /**
     * \brief Pushes `Nothing`.
     *
     * ### Format
     *
     *     PUSH_NOTHING
     *
     * ### Operation
     * - push Nothing
     *
     * ### Stack
     *
     *     (...) -> (..., Nothing)
     *
     * ### Registers
     * No change.
     */
    PushNothing,

    /**
     * \brief Pushes the value of a string literal.
     *
     * ### Format
     *
     *     PUSH_STRING <str>
     *
     * ### Operation
     * - let S = the (QoreString) value representing the string literal \<str\>
     * - increase reference count of S
     * - push S
     *
     * ### Stack
     *
     *     (...) -> (..., S)
     *
     * ### Registers
     * No change.
     */
    PushString,

    /**
     * \brief Pushes the value of a variable.
     *
     * ### Format
     *
     *     PUSH_VAR <var>
     *
     * ### Operation
     * - let V = the value of variable \<var\>
     * - increase reference count of V
     * - push V
     * - todo there will be locking involved unless var is local and not bound to any closure
     *
     * ### Stack
     *
     *     (...) -> (..., V)
     *
     * ### Registers
     * No change.
     */
    PushVar,

    /**
     * \brief Trims the value on the top of the stack.
     *
     * ### Format
     *
     *     TRIM
     *
     * ### Operation
     * - trim the value on the top of the stack
     * - assumes uniqueness of the value on the top of the stack
     *
     * ### Stack
     * No change, but must not be empty.
     *
     * ### Registers
     * No change.
     */
    Trim,
};

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_OPCODE_H_
