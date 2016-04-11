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
/// \brief TODO description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_EXPRESSIONANALYZER2_H_
#define INCLUDE_QORE_COMP_SEM_EXPRESSIONANALYZER2_H_

#include <vector>
#include "qore/common/Exceptions.h"
#include "qore/in/Interpreter.h"
#include "qore/ir/expr/Expression.h"
#include "qore/ir/expr/AssignmentExpression.h"
#include "qore/ir/expr/CompoundAssignmentExpression.h"
#include "qore/ir/expr/GlobalVariableRefExpression.h"
#include "qore/ir/expr/IntLiteralExpression.h"
#include "qore/ir/expr/InvokeExpression.h"
#include "qore/ir/expr/LifetimeStartExpression.h"
#include "qore/ir/expr/LocalVariableRefExpression.h"
#include "qore/ir/expr/NothingLiteralExpression.h"
#include "qore/ir/expr/StringLiteralRefExpression.h"
#include "qore/as/as.h"

namespace qore {
namespace comp {
namespace sem {

class FA {

public:
    explicit FA(as::F &f) : f(f) {
    }

    as::Id assignSlot(const ir::LocalVariable &lv) {
        assert(std::find(locals.begin(), locals.end(), &lv) == locals.end());
        auto it = std::find(locals.begin(), locals.end(), nullptr);
        if (it == locals.end()) {
            locals.push_back(&lv);
            return locals.size() - 1;
        }
        *it = &lv;
        return it - locals.begin();
    }

    as::Id findLocalSlot(const ir::LocalVariable &lv) {
        auto it = std::find(locals.begin(), locals.end(), &lv);
        assert(it != locals.end());
        return it - locals.begin();
    }

    as::Id getTemp() {
        if (!freeTemps.empty()) {
            as::Id t = freeTemps.back();
            freeTemps.pop_back();
            return t;
        }
        return f.tempCount++;
    }

    void doneTemp(as::Id temp) {
        freeTemps.push_back(temp);
    }

    void done() {
        f.localCount = locals.size();
    }

public:
    as::F &f;

private:
    std::vector<const ir::LocalVariable *> locals;
    std::vector<as::Id> freeTemps;
};


using LandingPad = as::Block *;

class Cleanup;


class LValueBase {
public:
    virtual ~LValueBase() = default;

    virtual void writeLock(as::Builder &b) = 0;
    virtual void writeUnlock(as::Builder &b) = 0;

    virtual void get(as::Builder &b, as::Id dest) = 0;
    virtual void set(as::Builder &b, as::Id src) = 0;

    virtual bool hasLock() = 0;
};

using LValue = std::unique_ptr<LValueBase>;

class Cleanup {

public:
    explicit Cleanup(FA &fa) : fa(fa), lValue(nullptr) {
    }

    void add(const ir::LocalVariable &lv) {
        locals.push_back(&lv);
    }

    void addTemp(as::Id temp) {
        auto it = std::find(temps.begin(), temps.end(), temp);
        assert(it == temps.end());
        temps.push_back(temp);
    }

    void removeTemp(as::Id temp) {
        auto it = std::find(temps.begin(), temps.end(), temp);
        assert(it != temps.end());
        temps.erase(it);
    }

    void setLValue(LValue &l) {
        assert(!lValue);
        lValue = l.get();
    }

    void unsetLValue(LValue &l) {
        assert(lValue == l.get());
        lValue = nullptr;
    }

    LandingPad getLandingPad() {
        if (temps.empty() && (!lValue || !lValue->hasLock()) && locals.empty()) {
            return nullptr;
        }
        as::Block *block = fa.f.createBlock();
        as::Builder b(block);

        as::Id e = fa.getTemp();
        b.createLandingPad(e);

        if (lValue) {
            lValue->writeUnlock(b);
        }

        for (auto it = temps.rbegin(); it != temps.rend(); ++it) {
            b.createRefDecNoexcept(*it, e);
        }

        for (auto it = locals.rbegin(); it != locals.rend(); ++it) {
            as::Id temp = fa.getTemp();
            b.createGetLocal(temp, fa.findLocalSlot(**it));
            b.createRefDecNoexcept(temp, e);
            fa.doneTemp(temp);
        }

        b.createRethrow(e);
        fa.doneTemp(e);
        return block;
    }

private:
    FA &fa;
    std::vector<as::Id> temps;
    std::vector<const ir::LocalVariable *> locals;
    LValueBase *lValue;
};

class LValueGlobal : public LValueBase {
public:
    explicit LValueGlobal(as::Id id) : id(id) {
    }

    void writeLock(as::Builder &b) override {
        b.createWriteLockGlobal(id);
    }

    void writeUnlock(as::Builder &b) override {
        b.createWriteUnlockGlobal(id);
    }

    void get(as::Builder &b, as::Id dest) override {
        //reference? any?
        b.createGetGlobal(dest, id);
    }

    void set(as::Builder &b, as::Id src) override {
        b.createSetGlobal(id, src);
    }

    bool hasLock() override {
        return true;
    }

private:
    as::Id id;
};

class LValueLocal : public LValueBase {
public:
    explicit LValueLocal(as::Id slot) : slot(slot) {
    }

    void writeLock(as::Builder &b) override {
    }

    void writeUnlock(as::Builder &b) override {
    }

    void get(as::Builder &b, as::Id dest) override {
        //reference? any?
        b.createGetLocal(dest, slot);
    }

    void set(as::Builder &b, as::Id src) override {
        b.createSetLocal(slot, src);
    }

    bool hasLock() override {
        return false;
    }

private:
    as::Id slot;
};

class ExpressionAnalyzer2 {

public:
    ExpressionAnalyzer2(FA &fa, as::Builder &b) : fa(fa), cleanup(fa), b(b) {
    }

public:
    void evaluate(as::Id dest, const ir::Expression &expr) {
        switch (expr.getKind()) {
//            Assignment,                   0
//            CompoundAssignment,           1
//            GlobalVariableRef,            2
//            IntLiteral,                   3
//            Invoke,                       4
//            InvokeBinaryOperator,         5
//            InvokeCionversion,            6
//            LifetimeStart,                7
//            LocalVariableRef,             8
//            NothingLiteral,               9
//            StringLiteralRef,             10

            case ir::Expression::Kind::GlobalVariableRef:
                evaluate(dest, static_cast<const ir::GlobalVariableRefExpression &>(expr));
                break;
            case ir::Expression::Kind::IntLiteral:
                evaluate(dest, static_cast<const ir::IntLiteralExpression &>(expr));
                break;
            case ir::Expression::Kind::InvokeBinaryOperator:
                evaluate(dest, static_cast<const ir::InvokeBinaryOperatorExpression &>(expr));
                break;
            case ir::Expression::Kind::InvokeConversion:
                evaluate(dest, static_cast<const ir::InvokeConversionExpression &>(expr));
                break;
            case ir::Expression::Kind::LocalVariableRef:
                evaluate(dest, static_cast<const ir::LocalVariableRefExpression &>(expr));
                break;
            case ir::Expression::Kind::StringLiteralRef:
                evaluate(dest, static_cast<const ir::StringLiteralRefExpression &>(expr));
                break;
            default:
                QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
        }
    }

    void evaluateNoValue(const ir::Expression &expr) {
        switch (expr.getKind()) {
            case ir::Expression::Kind::Assignment:
                evaluateNoValue(static_cast<const ir::AssignmentExpression &>(expr));
                return;
            case ir::Expression::Kind::CompoundAssignment:
                evaluateNoValue(static_cast<const ir::CompoundAssignmentExpression &>(expr));
                return;
            case ir::Expression::Kind::LifetimeStart:
                evaluateNoValue(static_cast<const ir::LifetimeStartExpression &>(expr));
                return;
            default:
                QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
        }
    }

    LValue evaluateLValue(const ir::Expression &expr) {
        switch (expr.getKind()) {
            case ir::Expression::Kind::GlobalVariableRef:
                return evaluateLValue(static_cast<const ir::GlobalVariableRefExpression &>(expr));
            case ir::Expression::Kind::LocalVariableRef:
                return evaluateLValue(static_cast<const ir::LocalVariableRefExpression &>(expr));
            default:
                QORE_UNREACHABLE("Invalid Expression::Kind: " << static_cast<int>(expr.getKind()));
        }
    }

private:
    void evaluate(as::Id dest, const ir::GlobalVariableRefExpression &expr) {
        LOG_FUNCTION();
        as::Id id = expr.getGlobalVariable().getId();
        b.createReadLockGlobal(id);
        b.createGetGlobal(dest, id);
        if (!expr.getGlobalVariable().getType().isPrimitive()) {
            b.createRefInc(dest);
        }
        b.createReadUnlockGlobal(id);
    }

    void evaluate(as::Id dest, const ir::IntLiteralExpression &expr) {
        LOG_FUNCTION();
        b.createIntConstant(dest, expr.getValue());
    }

    void evaluate(as::Id dest, const ir::InvokeBinaryOperatorExpression &expr) {
        LOG_FUNCTION();
        bool leftRef = !expr.getLeft().getType().isPrimitive();
        bool rightRef = !expr.getRight().getType().isPrimitive();
        bool retRef = !expr.getType().isPrimitive();

        as::Id left = fa.getTemp();
        evaluate(left, expr.getLeft());
        if (leftRef) {
            cleanup.addTemp(left);
        }

        as::Id right = fa.getTemp();
        evaluate(right, expr.getRight());
        if (rightRef) {
            cleanup.addTemp(right);
        }

        b.createBinaryOperator(dest, expr.getDesc(), left, right, cleanup.getLandingPad());

        if (retRef) {
            cleanup.addTemp(dest);
        }

        if (rightRef) {
            cleanup.removeTemp(right);
            b.createRefDec(right, cleanup.getLandingPad());
        }
        fa.doneTemp(right);

        if (leftRef) {
            cleanup.removeTemp(left);
            b.createRefDec(left, cleanup.getLandingPad());
        }
        fa.doneTemp(left);

        if (retRef) {
            cleanup.removeTemp(dest);
        }
    }

    void evaluate(as::Id dest, const ir::InvokeConversionExpression &expr) {
        LOG_FUNCTION();
        bool argRef = !expr.getArg().getType().isPrimitive();
        bool retRef = !expr.getType().isPrimitive();

        as::Id arg = fa.getTemp();
        evaluate(arg, expr.getArg());
        if (argRef) {
            cleanup.addTemp(arg);
        }

        b.createConversion(dest, expr.getDesc(), arg, cleanup.getLandingPad());

        if (retRef) {
            cleanup.addTemp(dest);
        }

        if (argRef) {
            cleanup.removeTemp(arg);
            b.createRefDec(arg, cleanup.getLandingPad());
        }
        fa.doneTemp(arg);

        if (retRef) {
            cleanup.removeTemp(dest);
        }
    }

    void evaluate(as::Id dest, const ir::LocalVariableRefExpression &expr) {
        LOG_FUNCTION();
        b.createGetLocal(dest, fa.findLocalSlot(expr.getLocalVariable()));
        if (!expr.getLocalVariable().getType().isPrimitive()) {
            b.createRefInc(dest);
        }
    }

    void evaluate(as::Id dest, const ir::StringLiteralRefExpression &expr) {
        LOG_FUNCTION();
        b.createLoadString(dest, expr.getStringLiteral().getId());
        b.createRefInc(dest);
    }

    void evaluateNoValue(const ir::AssignmentExpression &expr) {
        LOG_FUNCTION();
        bool isRef = !expr.getLeft().getType().isPrimitive();
        as::Id right = fa.getTemp();
        evaluate(right, expr.getRight());
        if (isRef) {
            cleanup.addTemp(right);
        }
        LValue left = evaluateLValue(expr.getLeft());
        left->writeLock(b);
        cleanup.setLValue(left);

        as::Id old = fa.getTemp();
        if (isRef) {
            left->get(b, old);
        }
        left->set(b, right);
        if (isRef) {
            cleanup.removeTemp(right);
            cleanup.addTemp(old);
        }
        fa.doneTemp(right);
        cleanup.unsetLValue(left);
        left->writeUnlock(b);
        if (isRef) {
            cleanup.removeTemp(old);
            b.createRefDec(old, cleanup.getLandingPad());
        }
        fa.doneTemp(old);
    }

    void evaluateNoValue(const ir::CompoundAssignmentExpression &expr) {
        LOG_FUNCTION();
        bool leftRef = !expr.getLeft().getType().isPrimitive();
        bool rightRef = !expr.getRight().getType().isPrimitive();

        as::Id right = fa.getTemp();
        evaluate(right, expr.getRight());
        if (rightRef) {
            cleanup.addTemp(right);
        }

        LValue left = evaluateLValue(expr.getLeft());
        left->writeLock(b);
        cleanup.setLValue(left);

        as::Id old = fa.getTemp();
        left->get(b, old);

        as::Id newValue = fa.getTemp();
        b.createBinaryOperator(newValue, expr.getDesc(), old, right, cleanup.getLandingPad());

        left->set(b, newValue);
        fa.doneTemp(newValue);
        if (leftRef) {
            cleanup.addTemp(old);
        }

        cleanup.unsetLValue(left);
        left->writeUnlock(b);
        if (leftRef) {
            cleanup.removeTemp(old);
            b.createRefDec(old, cleanup.getLandingPad());
        }
        fa.doneTemp(old);

        if (rightRef) {
            cleanup.removeTemp(right);
            b.createRefDec(right, cleanup.getLandingPad());
        }
        fa.doneTemp(right);
    }

    void evaluateNoValue(const ir::LifetimeStartExpression &expr) {
        LOG_FUNCTION();
        as::Id temp = fa.getTemp();
        evaluate(temp, expr.getInitExpression());
        as::Id slot = fa.assignSlot(expr.getLocalVariable());
        b.createSetLocal(slot, temp);
        fa.doneTemp(temp);
        if (!expr.getLocalVariable().getType().isPrimitive()) {
            cleanup.add(expr.getLocalVariable());
        }
    }

    LValue evaluateLValue(const ir::GlobalVariableRefExpression &expr) {
        LOG_FUNCTION();
        return LValue(new LValueGlobal(expr.getGlobalVariable().getId()));
    }

    LValue evaluateLValue(const ir::LocalVariableRefExpression &expr) {
        LOG_FUNCTION();
        return LValue(new LValueLocal(fa.findLocalSlot(expr.getLocalVariable())));
    }

private:
    FA &fa;
    Cleanup cleanup;
    as::Builder &b;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_EXPRESSIONANALYZER2_H_
