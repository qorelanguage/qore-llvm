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
#include "qore/pdp/CommentStripper.h"
#include "../Qtif.h"

namespace qore {
namespace pdp {

class CommentStripperTest : public qtif::LineTest {
};

TEST_P(CommentStripperTest, Run) {
    Source src(getFileName(), SourceId::Invalid, getInput(), false);
    CommentStripper<> cs(diagMgr, src);

    int c;
    while ((c = cs.read()) != Source::EndOfFile) {
        output << static_cast<char>(c);
    }
}

QTIF_TEST_CASE(CommentStripperTest, "pdp/cs/simple");

class CommentStripperExtTest : public qtif::LineTest {
};

TEST_P(CommentStripperExtTest, Run) {
    Source src(getFileName(), SourceId::Invalid, getInput(), false);
    CommentStripper<> cs(diagMgr, src);

    int c;
    do {
        SourceLocation loc = cs.getLocation();
        c = cs.read();
        if (c > 32 && c < 127) {
            output << static_cast<char>(c);
        } else {
            switch (c) {
                case Source::EndOfFile:
                    output << "\\eof";
                    break;
                case ' ':
                    output << "\\space";
                    break;
                case '\n':
                    output << "\\n";
                    break;
                default:
                    FAIL() << "Unknown character: " << c;
                    break;
            }
        }
        output << '@' << loc.line << ':' << loc.column << '\n';
    } while (c != Source::EndOfFile);
}

QTIF_TEST_CASE(CommentStripperExtTest, "pdp/cs/ext");

} // namespace pdp
} // namespace qore
