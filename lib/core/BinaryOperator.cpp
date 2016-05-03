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
/// \brief Implementation of BinaryOperator methods.
///
//------------------------------------------------------------------------------
#include "qore/core/BinaryOperator.h"

namespace qore {

std::ostream &operator<<(std::ostream &os, BinaryOperator::Kind kind) {
    switch (kind) {
        case BinaryOperator::Kind::Plus:
            return os << "+";
        case BinaryOperator::Kind::PlusEquals:
            return os << "+=";
        default:
            QORE_UNREACHABLE("Invalid BinaryOperator::Kind: " << static_cast<int>(kind));
    }
}

const BinaryOperator &BinaryOperator::find(Kind kind, const Type &left, const Type &right) {
    if (kind == Kind::Plus) {
        if (left == Type::Any || right == Type::Any) {
            return AnyPlusAny;
        }
        if (left == Type::String || right == Type::String) {
            return SoftStringPlusSoftString;
        }
        if (left == Type::Int || right == Type::Int) {
            return SoftIntPlusSoftInt;
        }
    }
    if (kind == Kind::PlusEquals) {
        if (left == Type::Any) {
            return AnyPlusEqualsAny;
        }
        if (left == Type::String) {
            return SoftStringPlusSoftString;
        }
        if (left == Type::Int) {
            return SoftIntPlusSoftInt;
        }
    }
    QORE_NOT_IMPLEMENTED("Operator " << left.getName() << " " << kind << " " << right.getName());
}

} //namespace qore
