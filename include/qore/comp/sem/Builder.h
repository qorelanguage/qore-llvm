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
/// \brief Code builder definition
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_BUILDER_H_
#define INCLUDE_QORE_COMP_SEM_BUILDER_H_

#include <map>
#include <string>
#include <vector>
#include "qore/core/Function.h"
#include "qore/core/code/Block.h"
#include "qore/core/util/Util.h"
#include "qore/comp/sem/GlobalVariableInfo.h"
#include "qore/comp/sem/LocalVariableInfo.h"

namespace qore {
namespace comp {
namespace sem {

class LValue;

/**
 * \brief Helper class for building code.
 *
 * Maintains the information about values (temps or local variables) that need their reference count decreased,
 * keeps track of the lvalue that needs unlocking and provides functions for building appropriate landing pad.
 */
class Builder {

public:
    virtual ~Builder() = default;

    ///\name Block management methods
    ///\{
    /**
     * \brief Creates a new \ref code::Block, but does not affect the 'current block'.
     * \return the new block
     */
    virtual code::Block *createBlock() = 0;

    /**
     * \brief Sets the 'current block' where new instructions will be appended.
     * \param block the block to append instructions to
     */
    void setCurrentBlock(code::Block *block) {
        currentBlock = block;
    }

    /**
     * \brief Returns true if the 'current block' is terminated.
     * \return true if the 'current block' is terminated
     */
    bool isTerminated() const {
        return currentBlock->isTerminated();
    }
    ///\}

    ///\name Temporary value management methods
    ///\{
    /**
     * \brief Allocates a temporary value. If possible, use \ref TempHelper instead.
     * \return the allocated temporary, must be deallocated by calling \ref setTempFree()
     */
    code::Temp getFreeTemp() {
        if (!freeTemps.empty()) {
            code::Temp t = freeTemps.back();
            freeTemps.pop_back();
            return t;
        }
        return createTemp();
    }

    /**
     * \brief Deallocates a temporary value. If possible, use \ref TempHelper instead.
     * \param temp a temporary allocated using \ref getFreeTemp()
     */
    void setTempFree(code::Temp temp) {
        freeTemps.push_back(temp);
    }

    /**
     * \brief Adds a temporary value to the list of values that need their reference count decreased.
     *
     * This list is used when building a landing pad.
     * \param temp the temporary to add
     */
    void derefNeeded(code::Temp temp) {
        assert(std::find(derefTemps.begin(), derefTemps.end(), temp) == derefTemps.end());
        derefTemps.push_back(temp);
    }

    /**
     * \brief Removes a temporary value from the list of values that need their reference count decreased.
     * \param temp the temporary to add
     */
    void derefDone(code::Temp temp) {
        auto it = std::find(derefTemps.begin(), derefTemps.end(), temp);
        assert(it != derefTemps.end());
        derefTemps.erase(it);
    }
    ///\}

    ///\name Lvalue lock tracking methods
    ///\{
    /**
     * \brief Sets the lvalue that needs unlocking.
     * \param lValue the lvalue
     */
    void unlockNeeded(LValue &lValue) {
        assert(unlockLValue == nullptr);
        unlockLValue = &lValue;
    }

    /**
     * \brief Clears the lvalue that needed unlocking.
     * \param lValue the lvalue
     */
    void unlockDone(LValue &lValue) {
        assert(unlockLValue == &lValue);
        unlockLValue = nullptr;
    }
    ///\}

    /**
     * \brief Pushes a local variable to the stack of locals that are in scope.
     * \param lv the local variable
     */
    void localsPush(const LocalVariableInfo &lv);

    ///\name Instruction creation methods
    ///\{
    void createBranch(code::Temp condition, const code::Block &trueDest, const code::Block &falseDest) {
        checkNotTerminated();
        currentBlock->appendBranch(condition, trueDest, falseDest);
        assert(isTerminated());
    }

    void createConstInt(code::Temp dest, qint value) {
        checkNotTerminated();
        currentBlock->appendConstInt(dest, value);
    }

    void createConstNothing(code::Temp dest) {
        checkNotTerminated();
        currentBlock->appendConstNothing(dest);
    }

    void createConstString(code::Temp dest, qore::String &string) {
        checkNotTerminated();
        currentBlock->appendConstString(dest, string);
    }

    void createGlobalGet(code::Temp dest, const GlobalVariableInfo &gv) {
        checkNotTerminated();
        currentBlock->appendGlobalGet(dest, gv.getRt());
    }

    void createGlobalInit(GlobalVariable &gv, code::Temp initValue) {
        checkNotTerminated();
        currentBlock->appendGlobalInit(gv, initValue);
    }

    void createGlobalReadLock(const GlobalVariableInfo &gv) {
        checkNotTerminated();
        currentBlock->appendGlobalReadLock(gv.getRt());
    }

    void createGlobalReadUnlock(const GlobalVariableInfo &gv) {
        checkNotTerminated();
        currentBlock->appendGlobalReadUnlock(gv.getRt());
    }

    void createGlobalSet(const GlobalVariableInfo &gv, code::Temp src) {
        checkNotTerminated();
        currentBlock->appendGlobalSet(gv.getRt(), src);
    }

    void createGlobalWriteLock(const GlobalVariableInfo &gv) {
        checkNotTerminated();
        currentBlock->appendGlobalWriteLock(gv.getRt());
    }

    void createGlobalWriteUnlock(const GlobalVariableInfo &gv) {
        checkNotTerminated();
        currentBlock->appendGlobalWriteUnlock(gv.getRt());
    }

    void createInvokeBinaryOperator(code::Temp dest, const BinaryOperator &op, code::Temp left, code::Temp right) {
        checkNotTerminated();
        currentBlock->appendInvokeBinaryOperator(dest, op, left, right, op.canThrow() ? getLandingPad() : nullptr);
    }

    void createInvokeConversion(code::Temp dest, const Conversion &conversion, code::Temp arg) {
        checkNotTerminated();
        currentBlock->appendInvokeConversion(dest, conversion, arg, conversion.canThrow() ? getLandingPad() : nullptr);
    }

    void createJump(const code::Block &dest) {
        checkNotTerminated();
        currentBlock->appendJump(dest);
        assert(isTerminated());
    }

    void createLocalGet(code::Temp dest, const LocalVariableInfo &lv) {
        checkNotTerminated();
        currentBlock->appendLocalGet(dest, lv.getRt());
    }

    void createLocalSet(const LocalVariableInfo &lv, code::Temp src) {
        checkNotTerminated();
        currentBlock->appendLocalSet(lv.getRt(), src);
    }

    void createRefDec(code::Temp temp) {    //XXX no need to create landing pad for simple types (e.g. string)
        checkNotTerminated();
        currentBlock->appendRefDec(temp, getLandingPad());
    }

    void createRefInc(code::Temp temp) {
        checkNotTerminated();
        currentBlock->appendRefInc(temp);
    }

    void createRet(code::Temp temp) {
        checkNotTerminated();
        buildLocalsDerefForRet();
        currentBlock->appendRet(temp);
        assert(isTerminated());
    }

    void createRetVoid() {
        checkNotTerminated();
        buildLocalsDerefForRet();
        currentBlock->appendRetVoid();
        assert(isTerminated());
    }

    void createRetVoidInteractive() {
        checkNotTerminated();
        currentBlock->appendRetVoid();
        assert(isTerminated());
    }
    ///\}

protected:
    /**
     * \brief Creates the builder. Derived classes must set currentBlock!
     */
    Builder() : currentBlock(nullptr), unlockLValue(nullptr) {
    }

    /**
     * \brief Creates a new temporary.
     * \return a new temporary
     */
    virtual code::Temp createTemp() = 0;

private:
    struct LocalsStackItem {
        explicit LocalsStackItem(const LocalVariable &lv, code::Block *b) : lv(&lv), mark(nullptr), b(b) {
        }

        explicit LocalsStackItem(const void *mark, code::Block *b) : lv(nullptr), mark(mark), b(b) {
        }

        const LocalVariable *lv;
        const void *mark;
        code::Block *b;
    };

private:
    void checkNotTerminated() {
        if (currentBlock->isTerminated()) {
            QORE_NOT_IMPLEMENTED("");   //report unreachable?
        }
    }

    code::Block *getLandingPad();
    code::Block *getLandingPad2(std::vector<LocalsStackItem>::reverse_iterator it);
    void buildLocalsDerefForRet();

private:
    code::Block *currentBlock;
    std::vector<code::Temp> freeTemps;
    std::vector<code::Temp> derefTemps;
    std::vector<LocalsStackItem> localsStack;
    LValue *unlockLValue;

    friend class LocalsStackMarker;
};

class FBuilder : public Builder {

public:
    explicit FBuilder(Function &f) : f(f) {
        setCurrentBlock(createBlock());
    }

    code::Temp createTemp() override {
        return f.addTemp();
    }

    code::Block *createBlock() override {
        return f.addBlock();
    }

private:
    Function &f;
};

class TempHelper {

public:
    explicit TempHelper(Builder &builder) : builder(builder), temp(builder.getFreeTemp()), needsDeref(false) {
    }

    ~TempHelper() {
        if (needsDeref) {
            builder.derefDone(temp);
            builder.createRefDec(temp);
        }
        builder.setTempFree(temp);
    }

    operator code::Temp() const {
        return temp;
    }

    void derefNeeded(bool isRef) {
        if (isRef) {
            needsDeref = true;
            builder.derefNeeded(temp);
        }
    }

    void derefDone() {
        if (needsDeref) {
            builder.derefDone(temp);
            needsDeref = false;
        }
    }

private:
    Builder &builder;
    code::Temp temp;
    bool needsDeref;
};

class LocalsStackMarker {

public:
    explicit LocalsStackMarker(Builder &builder, code::Block *catchBlock = nullptr) : builder(builder) {
        builder.localsStack.emplace_back(this, catchBlock);
    }

    ~LocalsStackMarker() {
        while (true) {
            Builder::LocalsStackItem item = builder.localsStack.back();
            builder.localsStack.pop_back();

            if (item.lv && item.lv->getType().isRefCounted() && !builder.isTerminated()) {
                TempHelper temp(builder);
                builder.currentBlock->appendLocalGet(temp, *item.lv);
                builder.currentBlock->appendRefDec(temp, builder.getLandingPad());
            }

            if (item.mark == this) {
                break;
            }
        }
    }

private:
    Builder &builder;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_BUILDER_H_
