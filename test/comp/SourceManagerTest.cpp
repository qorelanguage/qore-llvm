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
#include <sstream>
#include "gtest/gtest.h"
#include "qore/comp/SourceManager.h"
#include "MockDiagProcessor.h"

#define QTIF(a)
#include "test_files.inc"
#undef QTIF

namespace qore {
namespace comp {

struct SourceManagerTest : ::testing::Test, DiagManagerHelper {
    SourceManager mgr{diagMgr, std::string(TEST_INPUT_DIR) + "/"};

    std::string readAll(Source &src) {
        std::string s;
        while (true) {
            int c = src.read();
            if (c == 0) {
                return s;
            }
            s.push_back(c);
        }
    }
};

TEST_F(SourceManagerTest, FromString) {
    Source &src = mgr.createFromString("test", "xyz");
    EXPECT_EQ("xyz", readAll(src));
    EXPECT_EQ("test", src.getName());
}

TEST_F(SourceManagerTest, Get) {
    Source &src = mgr.createFromString("test", "xyz");
    EXPECT_EQ(&src, &mgr.get(src.getId()));
    EXPECT_THROW(mgr.get(src.getId() + 1), std::out_of_range);
}

TEST_F(SourceManagerTest, FromFile) {
    Source &src = mgr.createFromFile("SourceManagerTest_FromFile");
    EXPECT_EQ("abc", readAll(src));
    EXPECT_EQ(std::string(TEST_INPUT_DIR) + "/SourceManagerTest_FromFile", src.getName());
}

TEST_F(SourceManagerTest, FromFileErr) {
    EXPECT_CALL(mockDiagProcessor, process(MatchDiagRecordId(DiagId::CompScriptFileIoError))).Times(1);
    Source &src = mgr.createFromFile("SourceManagerTest_Nonexistent");
    EXPECT_EQ(0, src.read());
}

TEST_F(SourceManagerTest, NullCharWarnings) {
    EXPECT_CALL(mockDiagProcessor, process(MatchDiagRecordId(DiagId::CompNulCharactersIgnored))).Times(2);
    mgr.createFromString("test", std::string("x\0y\0z", 5));
}

} // namespace comp
} // namespace qore
