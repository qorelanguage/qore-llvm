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
#include "qore/BinaryOperator.h"
#include "qore/Conversion.h"
#include "qore/Type.h"
#include "qore/rt/BinaryOperators.h"
#include "qore/rt/Conversions.h"

namespace qore {

#define TYPE(KIND, NAME)     const Type Type::KIND(Type::Kind::KIND, NAME)
TYPE(Error,         "<error>");
TYPE(Any,           "any");
TYPE(Nothing,       "nothing");
TYPE(Bool,          "bool");
TYPE(SoftBool,      "softbool");
TYPE(Int,           "int");
TYPE(IntOpt,        "*int");
TYPE(SoftInt,       "softint");
TYPE(String,        "string");
TYPE(StringOpt,     "*string");
TYPE(SoftString,    "softstring");
#undef TYPE

#define CONVERSION2(NAME, FROM, TO, THROWS) const Conversion Conversion::NAME("convert" #NAME, \
        rt::convert ## NAME, Type::FROM, Type::TO, THROWS)
#define CONVERSION(FROM, TO, THROWS) CONVERSION2(FROM ## To ## TO, FROM, TO, THROWS)
CONVERSION(Any, String,     true);
CONVERSION(Int, Any,        true);
CONVERSION(Int, Bool,       false);
CONVERSION(Int, String,     true);
CONVERSION(String, Int,     true);
#undef CONVERSION
#undef CONVERSION2

#define BINOP2(NAME, KIND, LEFT, RIGHT, RESULT, THROWS)   const BinaryOperator BinaryOperator::NAME("binOp" #NAME, \
        rt::binOp ## NAME, BinaryOperator::Kind::KIND, Type::LEFT, Type::RIGHT, Type::RESULT, THROWS)
#define BINOP(LEFT, KIND, RIGHT, RESULT, THROWS) BINOP2(LEFT ## KIND ## RIGHT, KIND, LEFT, RIGHT, RESULT, THROWS)

BINOP(Any, Plus, Any, Any, true);
BINOP(Any, PlusEquals, Any, Any, true);
BINOP(SoftString, Plus, SoftString, String, true);
BINOP(SoftInt, Plus, SoftInt, Int, false);

#undef BINOP
#undef BINOP2
} // namespace qore
