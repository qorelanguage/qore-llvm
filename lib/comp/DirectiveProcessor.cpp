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
#include "qore/comp/DirectiveProcessor.h"
#include "qore/common/Logging.h"

namespace qore {
namespace comp {

DirectiveProcessor::DirectiveProcessor(DiagManager &diagMgr, SourceManager &srcMgr) : diagMgr(diagMgr), srcMgr(srcMgr),
    scanner(diagMgr) {
    LOG_FUNCTION();
}

void DirectiveProcessor::setSource(Source &src) {
    assert(srcStack.empty());
    srcStack.push(src);
}

Token DirectiveProcessor::read() {
    LOG_FUNCTION();
    assert(!srcStack.empty());
    while (true) {
        Token t = scanner.read(srcStack.top());
        switch (t.type) {
            case TokenType::EndOfFile:
                if (srcStack.size() == 1) {
                    return t;
                }
                srcStack.pop();
                break;
            case TokenType::Comment:
            case TokenType::None:
            case TokenType::Whitespace:
                break;
            case TokenType::PdInclude:
                processInclude();
                break;
            default:
                return t;
        }
    }
}

void DirectiveProcessor::processInclude() {
    Source &src = srcStack.top();
    src.setMark();
    SourceLocation l = src.getMarkLocation();   //TODO location
    std::string p = scanner.readDirectiveParam(src);
    srcStack.push(srcMgr.createFromFile(p, l));
}

} // namespace comp
} // namespace qore
