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
#include "qore/ir/decl/Function.h"

namespace qore {
namespace ir {

//FIXME remove this
BuiltinType Types::Any(rt::Type::Any, rt::qvalue_type::Ptr);
BuiltinType Types::Nothing(rt::Type::Nothing, rt::qvalue_type::None);
BuiltinType Types::Int(rt::Type::Int, rt::qvalue_type::Int);
BuiltinType Types::SoftInt(rt::Type::SoftInt, rt::qvalue_type::Int);
BuiltinType Types::String(rt::Type::String, rt::qvalue_type::Ptr);
BuiltinType Types::SoftString(rt::Type::SoftString, rt::qvalue_type::Ptr);

OperatorFunction Functions::StringPlusString(rt::Operator::StringPlusString,
        Types::String, Types::SoftString, Types::SoftString);
OperatorFunction Functions::IntPlusInt(rt::Operator::IntPlusInt,
        Types::Int, Types::SoftInt, Types::SoftInt);
OperatorFunction Functions::AnyPlusAny(rt::Operator::AnyPlusAny,
        Types::Any, Types::Any, Types::Any);
OperatorFunction Functions::AnyPlusEqAny(rt::Operator::AnyPlusEqAny,
        Types::Any, Types::Any, Types::Any);
ConversionFunction Functions::IntToString(rt::Conversion::IntToString, Types::String, Types::Int);
ConversionFunction Functions::StringToInt(rt::Conversion::StringToInt, Types::Int, Types::String);
ConversionFunction Functions::BoxInt(rt::Conversion::BoxInt, Types::Any, Types::Int);

} // namespace ir
} // namespace qore
