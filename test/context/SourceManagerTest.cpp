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
#include <sstream>
#include "qore/common/Util.h"
#include "qore/context/SourceManager.h"
#include "Helpers.h"

namespace qore {

struct SourceManagerTest : ::testing::Test, SourceIdTestHelper, SourceBufferTestHelper, SourceManagerTestHelper {
    SourceManager mgr;

};

TEST_F(SourceManagerTest, FromString) {
    SourceBuffer buf = mgr.createFromString("test", "xyz");
    EXPECT_FALSE(isStdin(buf));
    EXPECT_STREQ("xyz", getData(buf).data());
    EXPECT_EQ("test", getNames(mgr)[0]);
}

TEST_F(SourceManagerTest, FromStdin) {
    SourceBuffer buf = mgr.createFromStdin();
    EXPECT_TRUE(isStdin(buf));
    EXPECT_STREQ("\n", getData(buf).data());
    EXPECT_EQ("<stdin>", getNames(mgr)[0]);
}

TEST_F(SourceManagerTest, FromFile) {
    SourceBuffer buf = mgr.createFromFile("test/context/SourceManagerTest_FromFile");
    EXPECT_FALSE(isStdin(buf));
    EXPECT_STREQ("abc", getData(buf).data());
    EXPECT_EQ("test/context/SourceManagerTest_FromFile", getNames(mgr)[0]);
}

TEST_F(SourceManagerTest, FromFileErr) {
    auto before = getNames(mgr).size();
    EXPECT_THROW(mgr.createFromFile("test/context/SourceManagerTest_Nonexistent"), FatalError);
    EXPECT_EQ(before, getNames(mgr).size());
}

TEST_F(SourceManagerTest, GetName) {
    constexpr auto fileName("test/context/SourceManagerTest_FromFile");
    SourceBuffer sb1 = mgr.createFromString("buf1", "abc");
    SourceBuffer sb2 = mgr.createFromStdin();
    SourceBuffer sb3 = mgr.createFromFile(fileName);

    EXPECT_EQ(3U, getNames(mgr).size());
    EXPECT_EQ("buf1", mgr.getName(getSourceId(sb1)));
    EXPECT_EQ("<stdin>", mgr.getName(getSourceId(sb2)));
    EXPECT_EQ(fileName, mgr.getName(getSourceId(sb3)));
}

TEST_F(SourceManagerTest, FormatLocation) {
    mgr.createFromString("buf1", "abc");
    mgr.createFromString("buf2", "abc");
    std::ostringstream ss;

    EXPECT_EQ(ss, mgr.formatLocation(ss, {sourceId1, 12, 34}));
    EXPECT_EQ("buf2:12:34", ss.str());
}

} // namespace qore
