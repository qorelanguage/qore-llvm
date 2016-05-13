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
#include "qore/core/util/Logging.h"
#include "qore/core/util/Loggable.h"

namespace qore {
namespace util {

#ifdef QORE_LOGGING

class LoggableTest : public Loggable, public testing::Test {
protected:
    WRITE_TO_LOG("X");
};

TEST_F(LoggableTest, writeToLog) {
    std::ostringstream str;

    EXPECT_EQ(&str, &Loggable::writeToLog(str));
    EXPECT_EQ("abstract", str.str());
}

TEST_F(LoggableTest, streamOperatorRef) {
    std::ostringstream str;
    str << *this;
    EXPECT_EQ("X <0x", str.str().substr(0, 5));
}

TEST_F(LoggableTest, streamOperatorPtr) {
    std::ostringstream str;
    str << this;
    EXPECT_EQ("X <0x", str.str().substr(0, 5));
}

TEST_F(LoggableTest, streamOperatorPtrNullptr) {
    std::ostringstream str;
    LoggableTest *l = nullptr;
    str << l;
    EXPECT_EQ("<nullptr>", str.str());
}
#endif

} // namespace util
} // namespace qore
