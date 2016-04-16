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
#include "qore/comp/sem/FunctionOverloadPack.h"
#include "qore/common/Exceptions.h"
#include "ReportedError.h"

namespace qore {
namespace comp {
namespace sem {

void FunctionOverloadPack::pass2() {
    for (auto node : queue) {
        try {
            FunctionScope::Ptr ptr = util::make_unique<FunctionScope>(core, *this, *node);
            checkOverload(*ptr);
            //create block scope and core.addToQueue(scope, *node->body);
            functions.push_back(std::move(ptr));
        } catch (ReportedError &) {
            // ignored, diagnostic has been reported already
        }
    }
}

void FunctionOverloadPack::checkOverload(FunctionScope &f1) {
    for (auto &f2 : functions) {
        //if the type (in terms of overloading) of f1 is the same as f2, then report error and throw ReportedError
        QORE_NOT_IMPLEMENTED("overloads not supported "<< f2.get());
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
