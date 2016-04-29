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
#include "qore/core/Type.h"

namespace qore {

TEST(TypeTest, compare) {
    EXPECT_EQ(Type::String, Type::String);
    EXPECT_NE(Type::String, Type::StringOpt);
}

TEST(TypeTest, classTypes) {
    Type::Ptr t1 = Type::createForClass("A::B", false);
    EXPECT_EQ("A::B", t1->getName());
    EXPECT_FALSE(t1->acceptsNothing());
    EXPECT_TRUE(t1->isRefCounted());

    Type::Ptr t2 = Type::createForClass("A::B", true);
    EXPECT_EQ("*A::B", t2->getName());
    EXPECT_TRUE(t2->acceptsNothing());
    EXPECT_TRUE(t2->isRefCounted());

    EXPECT_EQ(*t1, *t1);
    EXPECT_EQ(*t2, *t2);
    EXPECT_NE(*t1, *t2);
}

TEST(TypeTest, builtinAcceptsNothing) {
    EXPECT_TRUE(Type::Error.acceptsNothing());
    EXPECT_TRUE(Type::Any.acceptsNothing());
    EXPECT_TRUE(Type::Nothing.acceptsNothing());
    EXPECT_FALSE(Type::Bool.acceptsNothing());
    EXPECT_FALSE(Type::SoftBool.acceptsNothing());
    EXPECT_FALSE(Type::Int.acceptsNothing());
    EXPECT_TRUE(Type::IntOpt.acceptsNothing());
    EXPECT_FALSE(Type::SoftInt.acceptsNothing());
    EXPECT_FALSE(Type::String.acceptsNothing());
    EXPECT_TRUE(Type::StringOpt.acceptsNothing());
    EXPECT_FALSE(Type::SoftString.acceptsNothing());
}

TEST(TypeTest, builtinIsRefCounted) {
    EXPECT_TRUE(Type::Error.isRefCounted());
    EXPECT_TRUE(Type::Any.isRefCounted());
    EXPECT_FALSE(Type::Nothing.isRefCounted());
    EXPECT_FALSE(Type::Bool.isRefCounted());
    EXPECT_FALSE(Type::SoftBool.isRefCounted());
    EXPECT_FALSE(Type::Int.isRefCounted());
    EXPECT_TRUE(Type::IntOpt.isRefCounted());
    EXPECT_FALSE(Type::SoftInt.isRefCounted());
    EXPECT_TRUE(Type::String.isRefCounted());
    EXPECT_TRUE(Type::StringOpt.isRefCounted());
    EXPECT_TRUE(Type::SoftString.isRefCounted());
}

TEST(TypeTest, builtinGetName) {
    EXPECT_EQ("<error>", Type::Error.getName());
    EXPECT_EQ("any", Type::Any.getName());
    EXPECT_EQ("nothing", Type::Nothing.getName());
    EXPECT_EQ("bool", Type::Bool.getName());
    EXPECT_EQ("softbool", Type::SoftBool.getName());
    EXPECT_EQ("int", Type::Int.getName());
    EXPECT_EQ("*int", Type::IntOpt.getName());
    EXPECT_EQ("softint", Type::SoftInt.getName());
    EXPECT_EQ("string", Type::String.getName());
    EXPECT_EQ("*string", Type::StringOpt.getName());
    EXPECT_EQ("softstring", Type::SoftString.getName());
}

} // namespace qore
