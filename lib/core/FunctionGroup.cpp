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
/// \brief Implementation of FunctionGroup methods.
///
//------------------------------------------------------------------------------
#include "qore/core/FunctionGroup.h"
#include <vector>

namespace qore {

OverloadResolutionResult FunctionGroup::resolveOverload(const std::vector<const Type *> argTypes) {
    if (functions.size() != 1) {
        QORE_NOT_IMPLEMENTED("");
    }
    Function &f = *functions[0];
    const FunctionType &t = f.getType();
    if (argTypes.size() != t.getParameterCount()) {
        QORE_NOT_IMPLEMENTED("");
    }
    std::vector<const Conversion *> conversions;
    for (Index i = 0; i < t.getParameterCount(); ++i) {
        const Conversion *c = Conversion::find(*argTypes[i], t.getParameterType(i));
        conversions.push_back(c);
    }
    return OverloadResolutionResult(f, std::move(conversions));
}

} //namespace qore
