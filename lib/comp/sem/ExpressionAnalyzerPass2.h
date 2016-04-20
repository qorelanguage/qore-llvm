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
#ifndef LIB_COMP_SEM_EXPRESSIONANALYZERPASS2_H_
#define LIB_COMP_SEM_EXPRESSIONANALYZERPASS2_H_

#include "qore/comp/sem/expr/AssignmentExpression.h"
#include "qore/comp/sem/expr/CompoundAssignmentExpression.h"
#include "qore/comp/sem/expr/GlobalVariableRefExpression.h"
#include "qore/comp/sem/expr/IntLiteralExpression.h"
#include "qore/comp/sem/expr/InvokeBinaryOperatorExpression.h"
#include "qore/comp/sem/expr/InvokeConversionExpression.h"
#include "qore/comp/sem/expr/LifetimeStartExpression.h"
#include "qore/comp/sem/expr/LocalVariableRefExpression.h"
#include "qore/comp/sem/expr/NothingLiteralExpression.h"
#include "qore/comp/sem/expr/StringLiteralRefExpression.h"

#include "LValue.h"

namespace qore {
namespace comp {
namespace sem {

class TempHelper {

public:
    explicit TempHelper(Builder &builder) : builder(builder), temp(builder.getFreeTemp()),
            inCleanup(false), doRefDec(false) {
    }

    ~TempHelper() {
        if (inCleanup) {
            builder.doneCleanup(temp);
        }
        if (doRefDec) {
            builder.createRefDec(temp);
        }
        builder.setTempFree(temp);
    }

    operator as::Temp() const {
        return temp;
    }

    void needsCleanup(bool isRef, bool doRefDec) {
        if (isRef) {
            inCleanup = true;
            this->doRefDec = doRefDec;
            builder.addCleanup(temp);
        }
    }

private:
    Builder &builder;
    as::Temp temp;
    bool inCleanup;
    bool doRefDec;
};

class ExpressionAnalyzerPass2 {

public:
    using ReturnType = void;

public:
    ExpressionAnalyzerPass2(Core &core, Builder &builder, as::Temp dest) : core(core), builder(builder),
            dest(dest), freeDest(false), destInCleanup(false) {
    }

    ExpressionAnalyzerPass2(Core &core, Builder &builder) : core(core), builder(builder),
            dest(InvalidId), freeDest(false), destInCleanup(false) {
    }

    ~ExpressionAnalyzerPass2() {
        if (destInCleanup) {
            builder.doneCleanup(dest);
        }
        if (freeDest) {
            builder.setTempFree(dest);
        }
    }

    void visit(const AssignmentExpression &expr) {
        withSideEffect();
        evaluate(dest, expr.getRight());
        if (!expr.getLeft().getType().isPrimitive()) {
            cleanupDest(true);
            TempHelper old(builder);
            LValue left(builder, expr.getLeft());
            left.get(old);
            old.needsCleanup(true, true);
            refIncDestIfNeeded();
            left.set(dest);
        } else {
            LValue left(builder, expr.getLeft());
            left.set(dest);
        }
    }

    void visit(const CompoundAssignmentExpression &expr) {
        withSideEffect();

        TempHelper right(builder);
        evaluate(right, expr.getRight());
        right.needsCleanup(!expr.getRight().getType().isPrimitive(), true);

        TempHelper old(builder);
        LValue left(builder, expr.getLeft());
        left.get(old);

        builder.createBinaryOperator(dest, expr.getDesc(), old, right);
        if (!expr.getLeft().getType().isPrimitive()) {
            cleanupDest(true);
            refIncDestIfNeeded();
            old.needsCleanup(true, true);
        }
        left.set(dest);
    }

    void visit(const GlobalVariableRefExpression &expr) {
        noSideEffect();
        const GlobalVariableInfo &gv = expr.getGlobalVariable();
        builder.createReadLockGlobal(gv);
        builder.createGetGlobal(dest, gv);
        if (!expr.getGlobalVariable().getType().isPrimitive()) {
            refIncDestIfNeeded();
        }
        builder.createReadUnlockGlobal(gv);
    }

    void visit(const IntLiteralExpression &expr) {
        noSideEffect();
        builder.createIntConstant(dest, expr.getValue());
    }

    void visit(const InvokeBinaryOperatorExpression &expr) {
        noSideEffect();

        TempHelper left(builder);
        evaluate(left, expr.getLeft());
        left.needsCleanup(!expr.getLeft().getType().isPrimitive(), true);

        TempHelper right(builder);
        evaluate(right, expr.getRight());
        right.needsCleanup(!expr.getRight().getType().isPrimitive(), true);

        builder.createBinaryOperator(dest, expr.getDesc(), left, right);
        cleanupDest(!expr.getType().isPrimitive());
    }

    void visit(const InvokeConversionExpression &expr) {
        noSideEffect();

        TempHelper arg(builder);
        evaluate(arg, expr.getArg());
        arg.needsCleanup(!expr.getArg().getType().isPrimitive(), true);

        builder.createConversion(dest, expr.getDesc(), arg);
        cleanupDest(!expr.getType().isPrimitive());
    }

    void visit(const LifetimeStartExpression &expr) {
        withSideEffect();
        evaluate(dest, expr.getInitExpression());
        if (!expr.getLocalVariable().getType().isPrimitive()) {
            refIncDestIfNeeded();
            builder.addCleanup(expr.getLocalVariable());
        }
        as::LocalSlot slot = builder.assignLocalSlot(expr.getLocalVariable());
        builder.createSetLocal(slot, dest);
    }

    void visit(const LocalVariableRefExpression &expr) {
        noSideEffect();
        builder.createGetLocal(dest, builder.findLocalSlot(expr.getLocalVariable()));
        if (!expr.getLocalVariable().getType().isPrimitive()) {
            refIncDestIfNeeded();
        }
    }

    void visit(const NothingLiteralExpression &expr) {
        noSideEffect();
        QORE_NOT_IMPLEMENTED("");
    }

    void visit(const StringLiteralRefExpression &expr) {
        noSideEffect();
        builder.createLoadString(dest, expr.getStringLiteral());
        builder.createRefInc(dest);
    }

private:
    void noSideEffect() {
        if (!dest.isValid()) {
            dest = builder.getFreeTemp();
            freeDest = true;
            QORE_NOT_IMPLEMENTED("report error");
            //core.ctx.report(DiagId::SemaStatementHasNoEffect, location);
        }
    }

    void withSideEffect() {
        if (!dest.isValid()) {
            dest = builder.getFreeTemp();
            freeDest = true;
        }
    }

    void refIncDestIfNeeded() {
        if (!freeDest) {
            builder.createRefInc(dest);
        } else {
            if (destInCleanup) {
                builder.doneCleanup(dest);
            }
            destInCleanup = false;
        }
    }

    void cleanupDest(bool isRef) {
        if (isRef) {
            builder.addCleanup(dest);
            destInCleanup = true;
        }
    }

    void evaluate(as::Temp temp, const Expression &e) {
        ExpressionAnalyzerPass2 a(core, builder, temp);
        e.accept(a);
    }

private:
    Core &core;
    Builder &builder;
    as::Temp dest;
    bool freeDest;
    bool destInCleanup;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_EXPRESSIONANALYZERPASS2_H_
