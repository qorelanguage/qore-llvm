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
#include "qore/core/util/Iterators.h"

namespace qore {
namespace util {

class ClassWithVectorAndMapOfPtrs {
public:
    using VectorIterator = VectorOfPtrIteratorAdapter<std::string, std::unique_ptr<std::string>>;
    using MapIterator = MapToPtrInteratorAdapter<int, std::string, std::unique_ptr<std::string>>;

    ClassWithVectorAndMapOfPtrs() {
        vector.push_back(std::unique_ptr<std::string>(new std::string("Item 1")));
        vector.push_back(std::unique_ptr<std::string>(new std::string("Item 2")));
        map[4] = std::unique_ptr<std::string>(new std::string("Item 1"));
        map[8] = std::unique_ptr<std::string>(new std::string("Item 2"));
    }

    VectorIterator begin() const {
        return VectorIterator(vector.begin());
    }

    VectorIterator end() const {
        return VectorIterator(vector.end());
    }

    IteratorRange<VectorIterator> getVector() const {
        return IteratorRange<VectorIterator>(vector);
    }

    IteratorRange<MapIterator> getMap() const {
        return IteratorRange<MapIterator>(map);
    }

private:
    std::vector<std::unique_ptr<std::string>> vector;
    std::map<int, std::unique_ptr<std::string>> map;
};

class IteratorsTest : public testing::Test {
};

TEST_F(IteratorsTest, vectorForEach) {
    ClassWithVectorAndMapOfPtrs c;
    std::ostringstream str;
    str << "|";
    for (const std::string &item : c) {
        str << item << "|";
    }
    EXPECT_EQ("|Item 1|Item 2|", str.str());
}

TEST_F(IteratorsTest, derefOperator) {
    ClassWithVectorAndMapOfPtrs c;
    auto it = c.begin();
    EXPECT_EQ(6U, it->length());
}

TEST_F(IteratorsTest, mapForEach) {
    ClassWithVectorAndMapOfPtrs c;
    std::ostringstream str;
    str << "|";
    for (const std::string &item : c.getMap()) {
        str << item << "|";
    }
    EXPECT_EQ("|Item 1|Item 2|", str.str());
}

TEST_F(IteratorsTest, rangeSize) {
    ClassWithVectorAndMapOfPtrs c;
    EXPECT_EQ(2, c.getVector().size());
}

} // namespace util
} // namespace qore
