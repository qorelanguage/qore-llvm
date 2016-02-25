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
#include "qore/comp/semantic/Analyzer.h"
#include "qore/comp/semantic/Dump.h"

namespace qore {
namespace comp {
namespace semantic {

class AnalyzerTest : public qtif::LineTest {
};

TEST_P(AnalyzerTest, Run) {
    DirectiveProcessor dp(diagMgr, srcMgr, getSrc());
    Parser parser(diagMgr, dp);
    ast::Script::Ptr script = parser.parseScript();
    Analyzer analyzer(srcMgr, diagMgr);
    std::unique_ptr<semantic::Namespace> root = analyzer.analyze(script);
    semantic::dump(srcMgr, output, root.get());
}

QTIF_TEST_CASE(AnalyzerTest, "semantic/");

TEST(DumpCoverage, Coverage) {
    struct MockSymbol : public Symbol {
        MOCK_CONST_METHOD0(getKind, Kind());
    };
    DiagManager diagMgr;
    SourceManager srcMgr(diagMgr);
    Dump<std::ostream> dump(srcMgr, std::cout);
    MockSymbol s;
    EXPECT_CALL(s, getKind()).WillOnce(::testing::Return(static_cast<Symbol::Kind>(999)));
    EXPECT_THROW(dump.dumpSymbol(&s), class Unreachable);
}

} // namespace semantic
} // namespace comp
} // namespace qore
