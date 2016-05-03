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
/// \brief TODO file description
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
#include "qore/comp/sem/CleanupScope.h"

namespace qore {
namespace comp {
namespace sem {

class LValue;

class Builder {

public:
    Builder() : currentBlock(createBlock()), cleanupLValue(nullptr), tempCount(0) {
        entry = currentBlock;
    }

    void build(Function &f) {
        f.setBody(tempCount, std::move(locals), std::move(blocks));
    }

    bool isTerminated() const {
        return currentBlock->isTerminated();
    }

    void startOfArgumentLifetime(Context &ctx, LocalVariableInfo &lv, Index argIndex) {
        const LocalVariable &aslv = alloc(ctx, lv);
        assert(aslv.getIndex() == argIndex);

        //if not shared:
        if (lv.getType().isRefCounted()) {
            code::Temp temp = getFreeTemp();
            currentBlock->appendLocalGet(temp, aslv);
            currentBlock->appendRefInc(temp);
            setTempFree(temp);
            x(aslv);
        }

        //if shared:
        // - get value from aslv to temp
        // - if not primitive, emit refInc
        // - emit instruction for allocating the wrapper with 'temp' as the initial value
        // - emit instruction for storing the wrapper ptr to local slot aslv
        // - push cleanup scope that dereferences the wrapper
    }

    void startOfLocalVariableLifetime(Context &ctx, LocalVariableInfo &lv, code::Temp value) {
        const LocalVariable &aslv = alloc(ctx, lv);

        //if not shared:
        currentBlock->appendLocalSet(aslv, value);
        if (lv.getType().isRefCounted()) {
            x(aslv);
        }

        //if shared:
        // - emit instruction for allocating the wrapper with 'value' as the initial value (its refcount is
        //          already increased)
        // - emit instruction for storing the wrapper ptr to local slot aslv
        // - push cleanup scope that dereferences the wrapper
    }

private:
    void x(const LocalVariable &lv) {
        code::Block *b = createBlock();
        code::Temp temp = getFreeTemp();      //all temps are free at this point
        b->appendLocalGet(temp, lv);
        b->appendRefDecNoexcept(temp);
        setTempFree(temp);
        code::Block *b2 = fff(cleanupScopes.rbegin());
        if (b2) {
            b->appendJump(*b2);
        } else {
            b->appendResumeUnwind();
        }
        cleanupScopes.emplace_back(lv, b);
    }

private:
    code::Block *fff(std::vector<CleanupScope>::reverse_iterator iit) {
        while (iit != cleanupScopes.rend()) {
            if (iit->b) {
                return iit->b;
            }
            ++iit;
        }
        return nullptr;
    }

public:
    void pushCleanupScope(const Statement &stmt, code::Block *catchBlock = nullptr) {
        cleanupScopes.emplace_back(stmt, catchBlock);
    }

    void popCleanupScope(const Statement &stmt) {
        while (true) {
            CleanupScope cs = cleanupScopes.back();
            cleanupScopes.pop_back();

            if (cs.lv) {
                if (!currentBlock->isTerminated() && cs.lv->getType().isRefCounted()) {
                    code::Temp temp = getFreeTemp();
                    currentBlock->appendLocalGet(temp, *cs.lv);
                    createRefDec(temp);
                    setTempFree(temp);
                }
            }

            if (cs.stmt == &stmt) {
                break;
            }
        }
    }

    void popCleanupScopes() {
        while (!cleanupScopes.empty()) {
            CleanupScope cs = cleanupScopes.back();
            cleanupScopes.pop_back();

            if (cs.lv) {
                if (!currentBlock->isTerminated() && cs.lv->getType().isRefCounted()) {
                    code::Temp temp = getFreeTemp();
                    currentBlock->appendLocalGet(temp, *cs.lv);
                    createRefDec(temp);
                    setTempFree(temp);
                }
            }
        }
    }

    void addCleanup(code::Temp temp) {
        LOG("ADD CLEANUP " << temp.getIndex());
        assert(std::find(cleanupTemps.begin(), cleanupTemps.end(), temp) == cleanupTemps.end());
        cleanupTemps.push_back(temp);
    }

    void doneCleanup(code::Temp temp) {
        LOG("DONE CLEANUP " << temp.getIndex());
        auto it = std::find(cleanupTemps.begin(), cleanupTemps.end(), temp);
        assert(it != cleanupTemps.end());
        cleanupTemps.erase(it);
    }

    void setCleanupLValue(LValue &lValue) {
        assert(cleanupLValue == nullptr);
        cleanupLValue = &lValue;
    }

    void clearCleanupLValue(LValue &lValue) {
        assert(cleanupLValue == &lValue);
        cleanupLValue = nullptr;
    }

    code::Temp getFreeTemp() {
        if (!freeTemps.empty()) {
            code::Temp t = freeTemps.back();
            freeTemps.pop_back();
            return t;
        }
        return code::Temp(tempCount++);
    }

    void setTempFree(code::Temp temp) {
        freeTemps.push_back(temp);
    }

    void createBranch(code::Temp condition, const code::Block &trueDest, const code::Block &falseDest) {
        checkNotTerminated();
        currentBlock->appendBranch(condition, trueDest, falseDest);
        assert(isTerminated());
    }

    void createConstInt(code::Temp dest, qint value) {
        checkNotTerminated();
        currentBlock->appendConstInt(dest, value);
    }

    void createConstString(code::Temp dest, qore::String::Ptr string) {
        checkNotTerminated();
        currentBlock->appendConstString(dest, std::move(string));
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

    void createRefDec(code::Temp temp) {
        assert(!cleanupLValue);
        checkNotTerminated();
        currentBlock->appendRefDec(temp, getLandingPad());
    }

    void createRefInc(code::Temp temp) {
        checkNotTerminated();
        currentBlock->appendRefInc(temp);
    }

    void createRet(code::Temp temp) {
        checkNotTerminated();
        buildCleanupForRet();
        currentBlock->appendRet(temp);
        assert(isTerminated());
    }

    void createRetVoid() {
        checkNotTerminated();
        buildCleanupForRet();
        currentBlock->appendRetVoid();
        assert(isTerminated());
    }



    code::Block &getEntryForInteractiveMode() {
        currentBlock->appendRetVoid();
        code::Block *b = entry;
        entry = createBlock();
        currentBlock = entry;
        return *b;
    }

    code::Block *createBlock() {
        code::Block::Ptr ptr = code::Block::Ptr(new code::Block());
        code::Block *b = ptr.get();
        blocks.push_back(std::move(ptr));
        return b;
    }

    void setCurrentBlock(code::Block *b) {
        currentBlock = b;
    }

private:
    void checkNotTerminated() {
        if (currentBlock->isTerminated()) {
            QORE_NOT_IMPLEMENTED("");   //report unreachable?
        }
    }

    code::Block *getLandingPad();
    code::Block *getLandingPad2(std::vector<CleanupScope>::reverse_iterator it);
    void buildCleanupForRet();

    const LocalVariable &alloc(Context &ctx, LocalVariableInfo &lv) {
        LocalVariable::Ptr ptr = util::make_unique<LocalVariable>(locals.size(),
                ctx.getString(lv.getName()), lv.getType());
        LocalVariable &v = *ptr;
        locals.push_back(std::move(ptr));
        lv.setRt(v);
        return v;
    }

private:
    std::vector<code::Temp> freeTemps;
    std::vector<code::Block::Ptr> blocks;
    code::Block *currentBlock;

    std::vector<code::Temp> cleanupTemps;
    std::vector<CleanupScope> cleanupScopes;
    LValue *cleanupLValue;
    std::vector<LocalVariable::Ptr> locals;

    Size tempCount;
    code::Block *entry;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_BUILDER_H_
