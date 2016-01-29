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
/// \brief Processor of parse directives.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_DIRECTIVEPROCESSOR_H_
#define INCLUDE_QORE_COMP_DIRECTIVEPROCESSOR_H_

#include <stack>
#include <functional>
#include "qore/comp/DiagManager.h"
#include "qore/comp/Scanner.h"
#include "qore/comp/SourceManager.h"

namespace qore {
namespace comp {

class DirectiveProcessor {

public:
    DirectiveProcessor(DiagManager &diagMgr, SourceManager &srcMgr);

    void setSource(Source &src);
    Token read();

private:
    void processInclude();

private:
    DiagManager &diagMgr;
    SourceManager &srcMgr;
    Scanner scanner;
    std::stack<std::reference_wrapper<Source>> srcStack;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_DIRECTIVEPROCESSOR_H_
