//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 - 2020 Qore Technologies, s.r.o.
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
/// \brief Token functions.
///
//------------------------------------------------------------------------------
#include "qore/comp/sem/TempHelper.h"
#include "qore/comp/sem/Builder.h"

namespace qore {
namespace comp {
namespace sem {

TempHelper::TempHelper(Builder &builder) : builder(&builder), temp(builder.getFreeTemp()),
        external(false), needsDeref(false) {
}

TempHelper::~TempHelper() {
    if (builder) {
        if (needsDeref) {
            builder->derefDone(temp);
            builder->createRefDec(temp);
        }
        if (!external) {
            builder->setTempFree(temp);
        }
    }
}

void TempHelper::derefNeeded(bool isRef) {
    assert(builder);
    if (isRef) {
        needsDeref = true;
        builder->derefNeeded(temp);
    }
}

/**
 * \brief Relinquishes the responsibility of dereferencing the temporary.
 */
void TempHelper::derefDone() {
    assert(builder);
    if (needsDeref) {
        builder->derefDone(temp);
        needsDeref = false;
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
