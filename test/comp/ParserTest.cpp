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
#include "qore/comp/DirectiveProcessor.h"
#include "qore/comp/Parser.h"
#include "qore/comp/ast/Dump.h"

namespace qore {
namespace comp {

class ParserTest : public qtif::LineTest {
};

TEST_P(ParserTest, Run) {
    DirectiveProcessor dp(ctx, getSrc());
    Parser parser(ctx, dp);
    ast::Script::Ptr script = parser.parseScript();
    ast::dump(ctx, output, *script);
}

QTIF_TEST_CASE(ParserTest, "parser/");

TEST(AstCoverage, ArgList) {
    Token t1;
    Token t2;
    t1.location.sourceId = 1;
    t1.location.offset = 2;
    t2.location.sourceId = 3;
    t2.location.offset = 4;
    ast::ArgList::Ptr a = ast::ArgList::create(t1, ast::ArgList::Data(), t2);
    EXPECT_EQ(1, a->getStart().sourceId);
    EXPECT_EQ(2, a->getStart().offset);
}

} // namespace comp
} // namespace qore
