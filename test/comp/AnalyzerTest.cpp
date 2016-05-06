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
#include "../Qtif.h"
#include "gmock/gmock.h"
#include "qore/comp/DirectiveProcessor.h"
#include "qore/comp/Parser.h"
#include "qore/comp/sem/Analyzer.h"
#include "qore/core/Env.h"
#include "qore/core/Dump.h"

namespace qore {
namespace comp {
namespace sem {

class AnalyzerTest : public qtif::LineTest {
};

TEST_P(AnalyzerTest, Run) {
    DirectiveProcessor dp(ctx, getSrc());
    Parser parser(ctx, dp);
    ast::Script::Ptr scriptNode = parser.parseScript();
    Core analyzer(ctx);
    Env env;
    auto r = analyze(ctx, env, *scriptNode);
    dump(output, env);
    if (r.first) {
        dump(output, "<qinit>", *r.first);
    }
    if (r.second) {
        dump(output, "<qmain>", *r.second);
    }
}

QTIF_TEST_CASE(AnalyzerTest, "semantic/");

} // namespace sem
} // namespace comp
} // namespace qore
