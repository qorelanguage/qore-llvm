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
/// \brief Visitor for the second pass of expression analysis.
///
//------------------------------------------------------------------------------
#ifndef LIB_COMP_SEM_EXPRESSIONANALYZERPASS2_H_
#define LIB_COMP_SEM_EXPRESSIONANALYZERPASS2_H_

#include <vector>
#include "qore/comp/sem/Builder.h"
#include "qore/comp/sem/expr/AssignmentExpression.h"
#include "qore/comp/sem/expr/CompoundAssignmentExpression.h"
#include "qore/comp/sem/expr/GlobalVariableRefExpression.h"
#include "qore/comp/sem/expr/IntLiteralExpression.h"
#include "qore/comp/sem/expr/InvokeBinaryOperatorExpression.h"
#include "qore/comp/sem/expr/InvokeConversionExpression.h"
#include "qore/comp/sem/expr/LocalVariableInitExpression.h"
#include "qore/comp/sem/expr/LocalVariableRefExpression.h"
#include "qore/comp/sem/expr/NothingLiteralExpression.h"
#include "qore/comp/sem/expr/StringLiteralRefExpression.h"
#include "LValue.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Translates a temporary representation of an expression to \ref qore::code.
 *
 * Mainly deals with proper reference counting of temporary values, checks for expression statements
 * with no side effect and checks for correct using of lvalues.
 *
 * **Implementation notes**
 *
 * - Each visitor methods must call either `withSideEffect()` or `noSideEffect()`. These make sure that
 * the `dest` member is a valid temporary identifier regardless of whether the caller provided it in constructor
 * or not. Also, `noSideEffect()` reports a 'statement has no effect' error if the called did not provide `dest`
 * in the constructor.
 * - Implementation relies on destructors of helper objects like LValue, TempHelper and ExpressionAnalyzerPass2 itself.
 * Their order is critical.
 */
class ExpressionAnalyzerPass2 {

public:
    /**
     * \brief Translates an expression to code that evaluates the expression into the given temporary.
     * \param core the shared state of the analyzer
     * \param builder code builder
     * \param dest the temporary that should have the value of the expression
     * \param node the expression to translate
     */
    static void eval(Core &core, Builder &builder, code::Temp dest, const Expression &node) {
        ExpressionAnalyzerPass2 a(core, builder, dest);
        node.accept(a);
    }

    /**
     * \brief Translates an expression and discards the resulting value. (The expression must have side effects.)
     * \param core the shared state of the analyzer
     * \param builder code builder
     * \param node the expression to translate
     */
    static void eval(Core &core, Builder &builder, const Expression &node) {
        ExpressionAnalyzerPass2 a(core, builder);
        node.accept(a);
    }

    ///\name Implementation of Expression visitor
    ///\{
    using ReturnType = void;

    void visit(const AssignmentExpression &expr) {
        evaluate(dest, expr.getRight());
        if (refCounted(expr.getLeft())) {
            dest.derefNeeded(true);
            TempHelper old(builder);
            LValue left(builder, expr.getLeft());
            left.get(old);
            old.derefNeeded(true);
            consumeDest();
            left.set(dest);
        } else {
            LValue left(builder, expr.getLeft());
            left.set(dest);
        }
    }

    void visit(const CompoundAssignmentExpression &expr) {
        TempHelper right(builder);
        evaluate(right, expr.getRight());
        right.derefNeeded(refCounted(expr.getRight()));

        TempHelper old(builder);
        LValue left(builder, expr.getLeft());
        left.get(old);

        builder.createInvokeBinaryOperator(dest, expr.getOperator(), old, right);
        if (refCounted(expr.getLeft())) {
            dest.derefNeeded(true);
            old.derefNeeded(true);
            consumeDest();
        }
        left.set(dest);
    }

    void visit(const FunctionCallExpression &expr) {
        std::vector<TempHelper> args;
        for (const Expression &arg : expr.getArgs()) {
            args.emplace_back(builder);
            evaluate(args.back(), arg);
            args.back().derefNeeded(refCounted(arg));
        }
        builder.createInvokeFunction(dest, expr.getFunction(), args);
        dest.derefNeeded(expr.getFunction().getType().getReturnType().isRefCounted());
    }

    void visit(const FunctionGroupExpression &expr) {
        QORE_UNREACHABLE("FunctionGroupExpression should be handled by pass 1");
    }

    void visit(const GlobalVariableRefExpression &expr) {
        noSideEffect();
        const GlobalVariableInfo &gv = expr.getGlobalVariable();
        builder.createGlobalReadLock(gv);
        builder.createGlobalGet(dest, gv);
        if (refCounted(expr.getGlobalVariable())) {
            builder.createRefInc(dest);
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
        left.derefNeeded(refCounted(expr.getLeft()));

        TempHelper right(builder);
        evaluate(right, expr.getRight());
        right.derefNeeded(refCounted(expr.getRight()));

        builder.createInvokeBinaryOperator(dest, expr.getOperator(), left, right);
        dest.derefNeeded(refCounted(expr));
    }

    void visit(const InvokeConversionExpression &expr) {
        noSideEffect();

        TempHelper arg(builder);
        evaluate(arg, expr.getArg());
        arg.derefNeeded(refCounted(expr.getArg()));

        builder.createInvokeConversion(dest, expr.getConversion(), arg);
        dest.derefNeeded(refCounted(expr));
    }

    void visit(const LocalVariableInitExpression &expr) {
        evaluate(dest, expr.getInitExpression());
        if (refCounted(expr.getLocalVariable())) {
            consumeDest();
        }

        //if not shared:
        builder.createLocalSet(expr.getLocalVariable(), dest);
        if (refCounted(expr.getLocalVariable())) {
            builder.localsPush(expr.getLocalVariable());
        }

        //if shared:
        // - emit instruction for allocating the wrapper with 'value' as the initial value (its refcount is
        //          already increased)
        // - emit instruction for storing the wrapper ptr to local slot aslv
        // - builder.localPush() that dereferences the wrapper
    }

    void visit(const LocalVariableRefExpression &expr) {
        noSideEffect();
        builder.createLocalGet(dest, expr.getLocalVariable());
        if (refCounted(expr.getLocalVariable())) {
            builder.createRefInc(dest);
        }
    }

    void visit(const NothingLiteralExpression &expr) {
        noSideEffect();
        builder.createConstNothing(dest);
    }

    void visit(const StringLiteralRefExpression &expr) {
        noSideEffect();
        builder.createConstString(dest, expr.getString());
        builder.createRefInc(dest);
    }
    ///\}

private:
    ExpressionAnalyzerPass2(Core &core, Builder &builder, code::Temp dest) : core(core), builder(builder),
            dest(builder, dest) {
    }

    ExpressionAnalyzerPass2(Core &core, Builder &builder) : core(core), builder(builder),
            dest(builder) {
    }

    ~ExpressionAnalyzerPass2() {
        if (dest.isExternallyProvided()) {
            dest.derefDone();
        }
    }

    template<typename T>
    static bool refCounted(T &t) {
        return t.getType().isRefCounted();
    }

    void noSideEffect() {
        if (!dest.isExternallyProvided()) {
            QORE_NOT_IMPLEMENTED("report error");
            //core.ctx.report(DiagId::SemaStatementHasNoEffect, location);
        }
    }

    void consumeDest() {
        if (dest.isExternallyProvided()) {
            builder.createRefInc(dest);
        } else {
            dest.derefDone();
        }
    }

    void evaluate(code::Temp temp, const Expression &e) {
        eval(core, builder, temp, e);
    }

private:
    Core &core;
    Builder &builder;
    TempHelper dest;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_EXPRESSIONANALYZERPASS2_H_
