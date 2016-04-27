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
#include "qore/Conversion.h"

namespace qore {

const Conversion *Conversion::find(const Type &src, const Type &dest) {
    //XXX can be replaced with a table
    if (src == dest) {
        return nullptr;
    }
    if (dest == Type::String) {
        if (src == Type::String) {
            return nullptr;
        }
        if (src == Type::Any) {
            return &AnyToString;
        }
    }
    if (dest == Type::SoftString) {
        if (src == Type::String) {
            return nullptr;
        }
        if (src == Type::Int) {
            return &IntToString;
        }
        if (src == Type::Any) {
            return &AnyToString;
        }
    }
    if (dest == Type::SoftBool) {
        if (src == Type::Int) {
            return &IntToBool;
        }
    }
    if (dest == Type::SoftInt) {
        if (src == Type::Int) {
            return nullptr;
        }
        if (src == Type::String) {
            return &StringToInt;
        }
    }
    if (dest == Type::Any) {
        if (src == Type::Int) {
            return &IntToAny;
        }
        if (src == Type::String) {
            return nullptr;
        }
    }
    QORE_NOT_IMPLEMENTED("Conversion " << src.getName() << " to " << dest.getName());

}

} // namespace qore
