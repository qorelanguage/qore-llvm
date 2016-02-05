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
#include "qore/comp/ast/Modifiers.h"

namespace qore {
namespace comp {
namespace ast {

class ModifiersTest : public ::testing::Test {
protected:
    Modifiers empty;
    Modifiers one{Modifier::Public};
    Modifiers two{Modifier::Public | Modifier::Final};
};

TEST_F(ModifiersTest, isEmpty) {
    EXPECT_TRUE(empty.isEmpty());
    EXPECT_FALSE(one.isEmpty());
}

TEST_F(ModifiersTest, contains) {
    EXPECT_TRUE(one.contains(Modifier::Public));
    EXPECT_FALSE(one.contains(Modifier::Private));
}

TEST_F(ModifiersTest, add) {
    Modifiers m = one | Modifier::Static;
    m |= Modifier::Final;
    EXPECT_TRUE(m.contains(Modifier::Public));
    EXPECT_TRUE(m.contains(Modifier::Static));
    EXPECT_TRUE(m.contains(Modifier::Final));
}

TEST_F(ModifiersTest, subset) {
    EXPECT_TRUE(empty.isSubsetOf(one));
    EXPECT_TRUE(one.isSubsetOf(two));
    EXPECT_TRUE(two.isSubsetOf(two));
    EXPECT_FALSE(two.isSubsetOf(one));
    EXPECT_TRUE(two.isSubsetOf(Modifier::Public | Modifier::Final | Modifier::Static));
}

TEST_F(ModifiersTest, toStream) {
    std::ostringstream s;
    s << Modifier::Abstract
            << "," << Modifier::Deprecated
            << "," << Modifier::Final
            << "," << Modifier::Private
            << "," << Modifier::Public
            << "," << Modifier::Static
            << "," << Modifier::Synchronized;
    EXPECT_EQ("abstract,deprecated,final,private,public,static,synchronized", s.str());
}

#ifdef QORE_COVERAGE
TEST_F(ModifiersTest, toStreamErr) {
    std::ostringstream ss;
    EXPECT_THROW(ss << static_cast<Modifier>(999), class Unreachable);
}
#endif

} // namespace ast
} // namespace comp
} // namespace qore
