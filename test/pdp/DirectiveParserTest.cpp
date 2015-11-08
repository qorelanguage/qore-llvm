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
#include "gtest/gtest.h"
#include "qore/pdp/DirectiveParser.h"
#include "../Qtif.h"
#include <sstream>

namespace qore {
namespace pdp {

class DirectiveParserTest : public qtif::LineTest {
public:
    void handleDirective(SourceLocation loc, DirectiveId id, SourceLocation argLoc, const std::string &arg) {
        std::ostringstream ss;
        ss << id;
        output << '#' << ss.str() << loc << '-' << arg << argLoc;
    }
};

TEST_P(DirectiveParserTest, Run) {
    Source src(getFileName(), SourceId::Invalid, getInput(), false);
    DirectiveParser<DirectiveParserTest> dp(diagMgr, *this, src);

    int c;
    while ((c = dp.read()) != Source::EndOfFile) {
        output << static_cast<char>(c);
    }
}

QTIF_TEST_CASE(DirectiveParserTest, "pdp/dp");

TEST(DirectivesTest, IdToStream) {
#define PD(I, N, A) \
    EXPECT_EQ(#I, static_cast<std::ostringstream&>(std::ostringstream().flush() << DirectiveId::I).str());
#include "qore/pdp/DirectivesData.inc"
#undef PD
}

#ifdef QORE_COVERAGE
TEST(DirectivesTest, IdToStreamErr) {
    std::ostringstream ss;
    EXPECT_THROW(ss << static_cast<DirectiveId>(999), class Unreachable);
}
#endif

//FIXME test comments in directives

} // namespace pdp
} // namespace qore
