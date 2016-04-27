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
#include "qore/common/Util.h"
#include "qore/comp/as/Temp.h"
#include "qore/comp/as/StringLiteral.h"
#include "qore/comp/as/GlobalVariable.h"
#include "qore/comp/as/LocalVariable.h"
#include "qore/comp/as/Block.h"
#include "qore/comp/as/Script.h"
#include "qore/comp/as/is.h"
#include "qore/comp/sem/GlobalVariableInfo.h"
#include "qore/comp/sem/LocalVariable.h"
#include "qore/comp/sem/CleanupScope.h"

namespace qore {
namespace comp {
namespace sem {

class LValue;

class Builder {

public:
    Builder() : currentBlock(createBlock()), terminated(false), cleanupLValue(nullptr), tempCount(0) {
        entry = currentBlock;
    }

    bool isTerminated() const {
        return terminated;
    }

    void startOfArgumentLifetime(const LocalVariable &lv, Id argIndex) {
        const as::LocalVariable &aslv = alloc(lv);
        assert(aslv.getId() == argIndex);

        //if not shared:
        if (lv.getType().isRefCounted()) {
            as::Temp temp = getFreeTemp();
            add(util::make_unique<as::GetLocal>(temp, aslv));
            add(util::make_unique<as::RefInc>(temp));
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

    void startOfLocalVariableLifetime(const LocalVariable &lv, as::Temp value) {
        const as::LocalVariable &aslv = alloc(lv);

        //if not shared:
        add(util::make_unique<as::SetLocal>(aslv, value));
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
    void x(const as::LocalVariable &lv) {
        as::Block *b = createBlock();
        as::Temp temp = getFreeTemp();      //all temps are free at this point
        b->instructions.push_back(util::make_unique<as::GetLocal>(temp, lv));
        b->instructions.push_back(util::make_unique<as::RefDecNoexcept>(temp));
        setTempFree(temp);
        as::Block *b2 = fff(cleanupScopes.rbegin());
        if (b2) {
            b->instructions.push_back(util::make_unique<as::Jump>(*b2));
        } else {
            b->instructions.push_back(util::make_unique<as::Rethrow>());
        }
        cleanupScopes.emplace_back(lv, b);
    }

private:
    as::Block *fff(std::vector<CleanupScope>::reverse_iterator iit) {
        while (iit != cleanupScopes.rend()) {
            if (iit->b) {
                return iit->b;
            }
            ++iit;
        }
        return nullptr;
    }

public:
    void pushCleanupScope(const Statement &stmt, as::Block *catchBlock = nullptr) {
        cleanupScopes.emplace_back(stmt, catchBlock);
    }

    void popCleanupScope(const Statement &stmt) {
        while (true) {
            CleanupScope cs = cleanupScopes.back();
            cleanupScopes.pop_back();

            if (cs.lv) {
                if (!terminated && cs.lv->getType().isRefCounted()) {
                    as::Temp temp = getFreeTemp();
                    add(util::make_unique<as::GetLocal>(temp, *cs.lv));
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
                if (!terminated && cs.lv->getType().isRefCounted()) {
                    as::Temp temp = getFreeTemp();
                    add(util::make_unique<as::GetLocal>(temp, *cs.lv));
                    createRefDec(temp);
                    setTempFree(temp);
                }
            }
        }
    }

    void addCleanup(as::Temp temp) {
        LOG("ADD CLEANUP " << temp.getIndex());
        assert(std::find(cleanupTemps.begin(), cleanupTemps.end(), temp.getIndex()) == cleanupTemps.end());
        cleanupTemps.push_back(temp.getIndex());
    }

    void doneCleanup(as::Temp temp) {
        LOG("DONE CLEANUP " << temp.getIndex());
        auto it = std::find(cleanupTemps.begin(), cleanupTemps.end(), temp.getIndex());
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

    as::Temp getFreeTemp() {
        if (!freeTemps.empty()) {
            as::Temp t = freeTemps.back();
            freeTemps.pop_back();
            return t;
        }
        return as::Temp(tempCount++);
    }

    void setTempFree(as::Temp temp) {
        freeTemps.push_back(temp);
    }

    void createRefInc(as::Temp temp) {
        add(util::make_unique<as::RefInc>(temp));
    }

    void createLoadString(as::Temp dest, qore::String::Ptr string) {
        add(util::make_unique<as::LoadString>(dest, std::move(string)));
    }

    void createMakeGlobal(const as::GlobalVariable &gv, as::Temp initValue) {
        add(util::make_unique<as::MakeGlobal>(gv, initValue));
    }

    void createRet(as::Temp temp) {
        buildCleanupForRet();
        add(util::make_unique<as::Ret>(temp));
        terminated = true;
    }

    void createRetVoid() {
        buildCleanupForRet();
        add(util::make_unique<as::RetVoid>());
        terminated = true;
    }

    void createSetLocal(const LocalVariable &lv, as::Temp src) {
        add(util::make_unique<as::SetLocal>(mapLocal(lv), src));
    }

    void createIntConstant(as::Temp dest, qint value) {
        add(util::make_unique<as::IntConstant>(dest, value));
    }

    void createGetLocal(as::Temp dest, const LocalVariable &lv) {
        add(util::make_unique<as::GetLocal>(dest, mapLocal(lv)));
    }

    void createRefDec(as::Temp temp) {
        assert(!cleanupLValue);
        add(util::make_unique<as::RefDec>(temp, getLandingPad()));
    }

    void createReadLockGlobal(const GlobalVariableInfo &gv) {
        add(util::make_unique<as::ReadLockGlobal>(gv.get()));
    }

    void createReadUnlockGlobal(const GlobalVariableInfo &gv) {
        add(util::make_unique<as::ReadUnlockGlobal>(gv.get()));
    }

    void createWriteLockGlobal(const GlobalVariableInfo &gv) {
        add(util::make_unique<as::WriteLockGlobal>(gv.get()));
    }

    void createWriteUnlockGlobal(const GlobalVariableInfo &gv) {
        add(util::make_unique<as::WriteUnlockGlobal>(gv.get()));
    }

    void createGetGlobal(as::Temp dest, const GlobalVariableInfo &gv) {
        add(util::make_unique<as::GetGlobal>(dest, gv.get()));
    }

    void createSetGlobal(const GlobalVariableInfo &gv, as::Temp src) {
        add(util::make_unique<as::SetGlobal>(gv.get(), src));
    }

    void createBinaryOperator(as::Temp dest, const BinaryOperator &op, as::Temp left, as::Temp right) {
        add(util::make_unique<as::BinaryOperator>(dest, op, left, right, op.canThrow() ? getLandingPad() : nullptr));
    }

    void createConversion(as::Temp dest, const qore::Conversion &conversion, as::Temp arg) {
        add(util::make_unique<as::Conversion>(dest, conversion, arg,
                conversion.canThrow() ? getLandingPad() : nullptr));
    }

    void createRefDecNoexcept(as::Temp temp) {
        add(util::make_unique<as::RefDecNoexcept>(temp));
    }

//    void createRethrow() {
//        add(util::make_unique<as::Rethrow>());
//        terminated = true;
//    }

    void createGetArg(as::Temp dest, Id index) {
        add(util::make_unique<as::GetArg>(dest, index));
    }

    void createJump(const as::Block &dest) {
        add(util::make_unique<as::Jump>(dest));
    }

    void createBranch(as::Temp condition, const as::Block &trueDest, const as::Block &falseDest) {
        add(util::make_unique<as::Branch>(condition, trueDest, falseDest));
    }


    as::Block &getEntryForInteractiveMode() {
        add(util::make_unique<as::RetVoid>());
        as::Block *b = entry;
        entry = createBlock();
        currentBlock = entry;
        terminated = false;
        return *b;
    }

    as::Block *createBlock() {
        as::Block::Ptr ptr = as::Block::create(blocks.size());
        as::Block *b = ptr.get();
        blocks.push_back(std::move(ptr));
        return b;
    }

    void setCurrentBlock(as::Block *b) {
        currentBlock = b;
        terminated = false;
    }

private:
    void add(as::Instruction::Ptr ins) {
        if (terminated) {
            QORE_NOT_IMPLEMENTED("");   //unreachable
        }
        currentBlock->instructions.push_back(std::move(ins));
    }

    as::Block *getLandingPad();
    as::Block *getLandingPad2(std::vector<CleanupScope>::reverse_iterator it);
    void buildCleanupForRet();

    const as::LocalVariable &alloc(const LocalVariable &lv) {
        as::LocalVariable::Ptr ptr = util::make_unique<as::LocalVariable>(locals.size(), lv.getName(),
                lv.getLocation(), lv.getType());
        as::LocalVariable *v = ptr.get();
        locals.push_back(std::move(ptr));
        localMap[&lv] = v;
        return *v;
    }

    const as::LocalVariable &mapLocal(const LocalVariable &lv) const {
        auto it = localMap.find(&lv);
        assert(it != localMap.end());
        return *it->second;
    }

private:
    std::vector<as::Temp> freeTemps;
    std::vector<as::Block::Ptr> blocks;
    as::Block *currentBlock;
    bool terminated;

    std::vector<Id> cleanupTemps;
    std::vector<CleanupScope> cleanupScopes;
    LValue *cleanupLValue;
    std::vector<as::LocalVariable::Ptr> locals;
    std::map<const LocalVariable *, const as::LocalVariable *> localMap;

    Id tempCount;
    as::Block *entry;

    friend class ScriptBuilder;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_BUILDER_H_
