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

    operator code::Temp() const {
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
    code::Temp temp;
    bool inCleanup;
    bool doRefDec;
};

class ExpressionAnalyzerPass2 {

public:
    using ReturnType = void;

    static constexpr Index InvalidIndex = -1;

public:
    ExpressionAnalyzerPass2(Core &core, Builder &builder, code::Temp dest) : core(core), builder(builder),
            dest(dest), freeDest(false), destInCleanup(false) {
    }

    ExpressionAnalyzerPass2(Core &core, Builder &builder) : core(core), builder(builder),
            dest(InvalidIndex), freeDest(false), destInCleanup(false) {
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
        if (refCounted(expr.getLeft())) {
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
        right.needsCleanup(refCounted(expr.getRight()), true);

        TempHelper old(builder);
        LValue left(builder, expr.getLeft());
        left.get(old);

        builder.createInvokeBinaryOperator(dest, expr.getOperator(), old, right);
        if (refCounted(expr.getLeft())) {
            cleanupDest(true);
            refIncDestIfNeeded();
            old.needsCleanup(true, true);
        }
        left.set(dest);
    }

    void visit(const GlobalVariableRefExpression &expr) {
        noSideEffect();
        const GlobalVariableInfo &gv = expr.getGlobalVariable();
        builder.createGlobalReadLock(gv);
        builder.createGlobalGet(dest, gv);
        if (refCounted(expr.getGlobalVariable())) {
            refIncDestIfNeeded();
        }
        builder.createGlobalReadUnlock(gv);
    }

    void visit(const IntLiteralExpression &expr) {
        noSideEffect();
        builder.createConstInt(dest, expr.getValue());
    }

    void visit(const InvokeBinaryOperatorExpression &expr) {
        noSideEffect();

        TempHelper left(builder);
        evaluate(left, expr.getLeft());
        left.needsCleanup(refCounted(expr.getLeft()), true);

        TempHelper right(builder);
        evaluate(right, expr.getRight());
        right.needsCleanup(refCounted(expr.getRight()), true);

        builder.createInvokeBinaryOperator(dest, expr.getOperator(), left, right);
        cleanupDest(refCounted(expr));
    }

    void visit(const InvokeConversionExpression &expr) {
        noSideEffect();

        TempHelper arg(builder);
        evaluate(arg, expr.getArg());
        arg.needsCleanup(refCounted(expr.getArg()), true);

        builder.createInvokeConversion(dest, expr.getConversion(), arg);
        cleanupDest(refCounted(expr));
    }

    void visit(const LifetimeStartExpression &expr) {
        withSideEffect();
        evaluate(dest, expr.getInitExpression());
        if (refCounted(expr.getLocalVariable())) {
            refIncDestIfNeeded();
        }
        builder.startOfLocalVariableLifetime(expr.getLocalVariable(), dest);
    }

    void visit(const LocalVariableRefExpression &expr) {
        noSideEffect();
        builder.createLocalGet(dest, expr.getLocalVariable());
        if (refCounted(expr.getLocalVariable())) {
            refIncDestIfNeeded();
        }
    }

    void visit(const NothingLiteralExpression &expr) {
        noSideEffect();
        QORE_NOT_IMPLEMENTED("");
    }

    void visit(const StringLiteralRefExpression &expr) {
        noSideEffect();
        builder.createConstString(dest, expr.getString());
        builder.createRefInc(dest);
    }

private:
    void noSideEffect() {
        if (dest.getIndex() == InvalidIndex) {
            dest = builder.getFreeTemp();
            freeDest = true;
            QORE_NOT_IMPLEMENTED("report error");
            //core.ctx.report(DiagId::SemaStatementHasNoEffect, location);
        }
    }

    void withSideEffect() {
        if (dest.getIndex() == InvalidIndex) {
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

    void evaluate(code::Temp temp, const Expression &e) {
        ExpressionAnalyzerPass2 a(core, builder, temp);
        e.accept(a);
    }

    template<typename T>
    static bool refCounted(T &t) {
        return t.getType().isRefCounted();
    }

private:
    Core &core;     //XXX is this really needed?
    Builder &builder;
    code::Temp dest;
    bool freeDest;
    bool destInCleanup;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_EXPRESSIONANALYZERPASS2_H_
