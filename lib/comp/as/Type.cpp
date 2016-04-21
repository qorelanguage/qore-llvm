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
#include "qore/comp/as/Type.h"

namespace qore {
namespace comp {
namespace as {

const Type Type::Error(rt::Type::Error, "<error>", false, false);
const Type Type::Bool(rt::Type::Bool, "bool", false, true);
const Type Type::SoftBool(rt::Type::SoftBool, "softbool", false, true);
const Type Type::Int(rt::Type::Int, "int", false, true);
const Type Type::IntOpt(rt::Type::Int, "int", true, false);
const Type Type::String(rt::Type::String, "string", false, false);
const Type Type::StringOpt(rt::Type::String, "string", true, false);
const Type Type::Any(rt::Type::Any, "any", false, false);
const Type Type::Nothing(rt::Type::Nothing, "nothing", false, true);  //primitive?

} // namespace as
} // namespace comp
} // namespace qore
