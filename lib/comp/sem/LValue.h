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

class LValue {

public:
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
        builder.setCleanupLValue(*this);
    }

    ~LValue() {
        builder.clearCleanupLValue(*this);
        cleanup(builder);
    }

    void cleanup(Builder &builder) {
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

    void get(code::Temp dest) {
        //reference? any?
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

    bool hasLock() {
        switch (expr.getKind()) {
            case Expression::Kind::GlobalVariableRef:
                return true;
            case Expression::Kind::LocalVariableRef:
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
