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
/// \brief Definition of a basic block.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_BLOCK_H_
#define INCLUDE_QORE_CORE_CODE_BLOCK_H_

#include <memory>
#include <vector>
#include "qore/core/code/Branch.h"
#include "qore/core/code/ConstInt.h"
#include "qore/core/code/ConstNothing.h"
#include "qore/core/code/ConstString.h"
#include "qore/core/code/GlobalGet.h"
#include "qore/core/code/GlobalInit.h"
#include "qore/core/code/GlobalReadLock.h"
#include "qore/core/code/GlobalReadUnlock.h"
#include "qore/core/code/GlobalSet.h"
#include "qore/core/code/GlobalWriteLock.h"
#include "qore/core/code/GlobalWriteUnlock.h"
#include "qore/core/code/InvokeBinaryOperator.h"
#include "qore/core/code/InvokeConversion.h"
#include "qore/core/code/Jump.h"
#include "qore/core/code/LocalGet.h"
#include "qore/core/code/LocalSet.h"
#include "qore/core/code/RefDec.h"
#include "qore/core/code/RefDecNoexcept.h"
#include "qore/core/code/RefInc.h"
#include "qore/core/code/ResumeUnwind.h"
#include "qore/core/code/Ret.h"
#include "qore/core/code/RetVoid.h"
#include "qore/core/util/Iterators.h"
#include "qore/core/util/Util.h"

namespace qore {
namespace code {

/**
 * \brief Represents a basic block.
 *
 * Basic block is an ordered collection of instructions that is executed as a whole (except when an
 * exception is thrown). Only the first instruction in the block can be the target of a jump. The last
 * instruction of a block must be a terminator - a jump, return, throw etc. The only form of flow control allowed in
 * the middle of a block is in the form of landing pads for exceptions.
 */
class Block {

public:
    using Ptr = std::unique_ptr<Block>;                                     //!< Pointer type.
    using Iterator = util::VectorOfUniquePtrIteratorAdapter<Instruction>;   //!< Iterator type.

public:
    /**
     * \brief Default constructor.
     */
    Block() {
    }

    /**
     * \brief Returns an iterator positioned to the first instruction in the block.
     * \return an iterator positioned to the first instruction in the block
     */
    Iterator begin() const {
        return Iterator(instructions.begin());
    }

    /**
     * \brief Returns an iterator positioned past the last instruction of the block.
     * \return an iterator positioned past the last instruction of the block
     */
    Iterator end() const {
        return Iterator(instructions.end());
    }

    /**
     * \brief Returns true if the last instruction in the block is a terminator.
     * \return true if the last instruction in the block is a terminator
     */
    bool isTerminated() const {
        return !instructions.empty() && instructions.back()->isTerminator();
    }

    /**
     * \brief Appends a Branch instruction to the end of the block.
     * \param condition the temporary holding the value of the condition (must be a boolean value)
     * \param trueDest the block to jump to in case the condition is true
     * \param falseDest the block to jump to in case the condition is false
     */
    void appendBranch(Temp condition, const Block &trueDest, const Block &falseDest) {
        append(util::make_unique<Branch>(condition, trueDest, falseDest));
    }

    /**
     * \brief Appends a ConstInt instruction to the end of the block.
     * \param dest the temporary to load the constant into
     * \param value the constant value
     */
    void appendConstInt(Temp dest, qint value) {
        append(util::make_unique<ConstInt>(dest, value));
    }

    /**
     * \brief Appends a ConstNothing instruction to the end of the block.
     * \param dest the temporary to load the constant into
     */
    void appendConstNothing(Temp dest) {
        append(util::make_unique<ConstNothing>(dest));
    }

    /**
     * \brief Appends a ConstString instruction to the end of the block.
     * \param dest the temporary to load the constant into
     * \param value the constant value
     */
    void appendConstString(Temp dest, String &value) {
        append(util::make_unique<ConstString>(dest, value));
    }

    /**
     * \brief Appends a GlobalGet instruction to the end of the block.
     * \param dest the temporary to load the value of the global variable into
     * \param globalVariable the global variable
     */
    void appendGlobalGet(Temp dest, GlobalVariable &globalVariable) {
        append(util::make_unique<GlobalGet>(dest, globalVariable));
    }

    /**
     * \brief Appends a GlobalInit instruction to the end of the block.
     * \param globalVariable the global variable
     * \param initValue the temporary with the initial value
     */
    void appendGlobalInit(GlobalVariable &globalVariable, Temp initValue) {
        append(util::make_unique<GlobalInit>(globalVariable, initValue));
    }

    /**
     * \brief Appends a GlobalReadLock instruction to the end of the block.
     * \param globalVariable the global variable
     */
    void appendGlobalReadLock(GlobalVariable &globalVariable) {
        append(util::make_unique<GlobalReadLock>(globalVariable));
    }

    /**
     * \brief Appends a GlobalReadUnlock instruction to the end of the block.
     * \param globalVariable the global variable
     */
    void appendGlobalReadUnlock(GlobalVariable &globalVariable) {
        append(util::make_unique<GlobalReadUnlock>(globalVariable));
    }

    /**
     * \brief Appends a GlobalSet instruction to the end of the block.
     * \param globalVariable the global variable
     * \param src the temporary to store into the global variable
     */
    void appendGlobalSet(GlobalVariable &globalVariable, Temp src) {
        append(util::make_unique<GlobalSet>(globalVariable, src));
    }

    /**
     * \brief Appends a GlobalWriteLock instruction to the end of the block.
     * \param globalVariable the global variable
     */
    void appendGlobalWriteLock(GlobalVariable &globalVariable) {
        append(util::make_unique<GlobalWriteLock>(globalVariable));
    }

    /**
     * \brief Appends a GlobalWriteUnlock instruction to the end of the block.
     * \param globalVariable the global variable
     */
    void appendGlobalWriteUnlock(GlobalVariable &globalVariable) {
        append(util::make_unique<GlobalWriteUnlock>(globalVariable));
    }

    /**
     * \brief Appends an InvokeBinaryOperator instruction to the end of the block.
     * \param dest the destination temporary
     * \param op the binary operator
     * \param left the temporary holding the left operand
     * \param right the temporary holding the right operand
     * \param lpad optional landing pad in case an exception occurs
     */
    void appendInvokeBinaryOperator(Temp dest, const BinaryOperator &op, Temp left, Temp right, const Block *lpad) {
        append(util::make_unique<InvokeBinaryOperator>(dest, op, left, right, lpad));
    }

    /**
     * \brief Appends an InvokeConversion instruction to the end of the block.
     * \param dest the destination temporary
     * \param conversion the conversion
     * \param arg the temporary holding the value to convert
     * \param lpad optional landing pad in case an exception occurs
     */
    void appendInvokeConversion(Temp dest, const Conversion &conversion, Temp arg, const Block *lpad) {
        append(util::make_unique<InvokeConversion>(dest, conversion, arg, lpad));
    }

    /**
     * \brief Appends a Jump instruction to the end of the block.
     * \param dest the block to jump to
     */
    void appendJump(const Block &dest) {
        append(util::make_unique<Jump>(dest));
    }

    /**
     * \brief Appends a LocalGet instruction to the end of the block.
     * \param dest the temporary to load the value of the local variable into
     * \param localVariable the local variable
     */
    void appendLocalGet(Temp dest, const LocalVariable &localVariable) {
        append(util::make_unique<LocalGet>(dest, localVariable));
    }

    /**
     * \brief Appends a LocalSet instruction to the end of the block.
     * \param localVariable the local variable
     * \param src the temporary to store into the local variable
     */
    void appendLocalSet(const LocalVariable &localVariable, Temp src) {
        append(util::make_unique<LocalSet>(localVariable, src));
    }

    /**
     * \brief Appends a RefDec instruction to the end of the block.
     * \param temp the temporary value whose reference count should be decreased
     * \param lpad optional landing pad in case an exception occurs
     */
    void appendRefDec(Temp temp, const Block *lpad) {
        append(util::make_unique<RefDec>(temp, lpad));
    }

    /**
     * \brief Appends a RefDecNoexcept instruction to the end of the block.
     * \param temp the temporary value whose reference count should be decreased
     */
    void appendRefDecNoexcept(Temp temp) {
        append(util::make_unique<RefDecNoexcept>(temp));
    }

    /**
     * \brief Appends a RefInc instruction to the end of the block.
     * \param temp the temporary value whose reference count should be increased
     */
    void appendRefInc(Temp temp) {
        append(util::make_unique<RefInc>(temp));
    }

    /**
     * \brief Appends a ResumeUnwind instruction to the end of the block.
     */
    void appendResumeUnwind() {
        append(util::make_unique<ResumeUnwind>());
    }

    /**
     * \brief Appends a RetVoid instruction to the end of the block.
     */
    void appendRetVoid() {
        append(util::make_unique<RetVoid>());
    }

    /**
     * \brief Appends a Ret instruction to the end of the block.
     * \param value the temporary holding the return value
     */
    void appendRet(Temp value) {
        append(util::make_unique<Ret>(value));
    }

private:
    void append(Instruction::Ptr instruction) {
        assert(!isTerminated());
        instructions.push_back(std::move(instruction));
    }

private:
    Block(const Block &) = delete;
    Block(Block &&) = delete;
    Block &operator=(const Block &) = delete;
    Block &operator=(Block &&) = delete;

private:
    std::vector<Instruction::Ptr> instructions;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_BLOCK_H_
