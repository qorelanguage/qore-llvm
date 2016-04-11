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
#include "qore/rt/Meta.h"
#include "qore/rt/Func.h"
#include "qore/common/Exceptions.h"

namespace qore {
namespace rt {
namespace meta {

static qvalue conv_id(qvalue v) {
    return v;
}

#define C(C, F, RET, ARG)           { Conversion::C, F, Type::RET, Type::ARG }
#define BO(F, RET, LEFT, RIGHT)     { F, Type::RET, Type::LEFT, Type::RIGHT }

//Warning - entries must be in the same order as rt::Conversion
ConversionDesc ConversionTable[] = {
        C(Identity, conv_id, Any, Any),
        C(IntToString, convertIntToString, String, Int),
        C(StringToInt, convertStringToInt, Int, String),
        C(BoxInt, int_box, Any, Int),
};

//Warning - entries must be in the same order as rt::Operator
BinaryOperatorDesc BinaryOperatorTable[] = {
        BO(opAddIntInt, Int, SoftInt, SoftInt),
        BO(opAddStringString, String, SoftString, SoftString),
        BO(op_add_any_any, Any, Any, Any),
        BO(op_addeq_any_any, Any, Any, Any),
};

#undef C
#undef BO

static Operator findOperatorImpl(Op o, Type l, Type r) {
    if (o == Op::Plus) {
        if (l == Type::Any || r == Type::Any) {
            return Operator::AnyPlusAny;
        }
        if (l == Type::String || r == Type::String) {
            return Operator::StringPlusString;
        }
        if (l == Type::Int || r == Type::Int) {
            return Operator::IntPlusInt;
        }
    }
    if (o == Op::PlusEq) {
        if (l == Type::Any) {
            return Operator::AnyPlusEqAny;
        }
        if (l == Type::String) {
            return Operator::StringPlusString;
        }
        if (l == Type::Int) {
            return Operator::IntPlusInt;
        }
    }
    QORE_NOT_IMPLEMENTED("Operator " << static_cast<int>(l) << " + " << static_cast<int>(r));
}

const BinaryOperatorDesc &findOperator(Op o, Type l, Type r) {
    return rt::meta::BinaryOperatorTable[static_cast<int>(findOperatorImpl(o, l, r))];
}

Conversion findConversionImpl(Type src, Type dest) {
    //XXX can be replaced with a table
    if (src == dest) {
        return Conversion::Identity;
    }
    if (dest == Type::SoftString) {
        if (src == Type::String) {
            return Conversion::Identity;
        }
        if (src == Type::Int) {
            return Conversion::IntToString;
        }
    }
    if (dest == Type::SoftInt) {
        if (src == Type::String) {
            return Conversion::StringToInt;
        }
        if (src == Type::Int) {
            return Conversion::Identity;
        }
    }
    if (dest == Type::Any) {
        if (src == Type::Int) {
            return Conversion::BoxInt;
        }
        if (src == Type::String) {
            return Conversion::Identity;
        }
    }
    QORE_NOT_IMPLEMENTED("Conversion " << static_cast<int>(src) << " to " << static_cast<int>(dest));
}

const ConversionDesc &findConversion(Type src, Type dest) {
    return rt::meta::ConversionTable[static_cast<int>(findConversionImpl(src, dest))];
}

} // namespace meta
} // namespace rt
} // namespace qore
