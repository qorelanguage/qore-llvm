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
#ifndef LIB_COMP_SEM_LVALUE_H_
#define LIB_COMP_SEM_LVALUE_H_

#include "qore/comp/sem/expr/GlobalVariableRefExpression.h"
#include "qore/comp/sem/expr/LocalVariableRefExpression.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Helper class for translating lvalues.
 */
class LValue {

public:
    /**
     * \brief Generates code for locking the lvalue.
     * \param builder the code builder
     * \param expr the lvalue expression
     */
    LValue(Builder &builder, const Expression &expr) : builder(builder), expr(expr) {
        switch (expr.getKind()) {
            case Expression::Kind::GlobalVariableRef:
                builder.createGlobalWriteLock(asGlobal().getGlobalVariable());
                break;
            case Expression::Kind::LocalVariableRef:
                //TODO lock if shared
                break;
            default:
                QORE_NOT_IMPLEMENTED("LValue: Expression::Kind " << static_cast<int>(expr.getKind()));
        }
        builder.unlockNeeded(*this);
    }

    ~LValue() {
        builder.unlockDone(*this);
        unlock(builder);
    }

    /**
     * \brief Generates code for unlocking the lvalue.
     * \param builder the code builder
     */
    void unlock(Builder &builder) {
        switch (expr.getKind()) {
            case Expression::Kind::GlobalVariableRef:
                builder.createGlobalWriteUnlock(asGlobal().getGlobalVariable());
                break;
            case Expression::Kind::LocalVariableRef:
                //TODO unlock if shared
                break;
            default:
                //error reported in ctor
                break;
        }
    }

    /**
     * \brief Generates code for getting the old value.
     * \param dest the destination temporary for the old value
     */
    void get(code::Temp dest) {
        switch (expr.getKind()) {
            case Expression::Kind::GlobalVariableRef:
                builder.createGlobalGet(dest, asGlobal().getGlobalVariable());
                break;
            case Expression::Kind::LocalVariableRef:
                builder.createLocalGet(dest, asLocal().getLocalVariable());
                break;
            default:
                //error reported in ctor
                break;
        }
    }

    /**
     * \brief Generates code for setting the new value.
     * \param src the temporary with the new value
     */
    void set(code::Temp src) {
        switch (expr.getKind()) {
            case Expression::Kind::GlobalVariableRef:
                builder.createGlobalSet(asGlobal().getGlobalVariable(), src);
                break;
            case Expression::Kind::LocalVariableRef:
                builder.createLocalSet(asLocal().getLocalVariable(), src);
                break;
            default:
                //error reported in ctor
                break;
        }
    }

    /**
     * \brief Returns true iff \ref unlock() generates at least one instruction.
     * \return true iff \ref unlock() generates at least one instruction
     */
    bool hasLock() {
        switch (expr.getKind()) {
            case Expression::Kind::GlobalVariableRef:
                return true;
            case Expression::Kind::LocalVariableRef:
                //TODO true if shared
                return false;
            default:
                return false;
        }
    }

private:
    const GlobalVariableRefExpression &asGlobal() {
        assert(expr.getKind() == Expression::Kind::GlobalVariableRef);
        return static_cast<const GlobalVariableRefExpression &>(expr);
    }

    const LocalVariableRefExpression &asLocal() {
        assert(expr.getKind() == Expression::Kind::LocalVariableRef);
        return static_cast<const LocalVariableRefExpression &>(expr);
    }

private:
    Builder &builder;
    const Expression &expr;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // LIB_COMP_SEM_LVALUE_H_
