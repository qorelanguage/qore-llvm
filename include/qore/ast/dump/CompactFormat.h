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
///
/// \file
/// \brief TODO File description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_DUMP_COMPACTFORMAT_H_
#define INCLUDE_QORE_AST_DUMP_COMPACTFORMAT_H_

#include <iostream>
#include <string>
#include <vector>
#include "qore/ast/dump/Tags.h"

namespace qore {
namespace ast {
namespace dump {

struct IndentEx {
    IndentEx &operator++() {
        stack.push_back(2);
        return *this;
    }

    IndentEx &operator--() {
        stack.pop_back();
        return *this;
    }

    std::vector<int> stack;
};


inline std::ostream &operator<<(std::ostream &os, IndentEx &indent) {
    for (auto &s : indent.stack) {
        switch (s) {
            case 1:
                os << "\u2502 ";
                break;
            case 2:
                os << "\u251c\u2500";
                s = 1;
                break;
            case 3:
                os << "\u2514\u2500";
                s = 4;
                break;
            case 4:
                os << "  ";
                break;
        }
    }
    return os;
}

class CompactFormat {

public:
    CompactFormat &operator<<(Last) {
        last = true;
        return *this;
    }

    CompactFormat &operator<<(BeginNode type) {
        std::cout << indent << (type || savedChildName);
        inHeader = true;
        return *this;
    }

    template<typename T>
    CompactFormat &operator<<(Attribute<T> attr) {
        std::cout << " " << attr.name << ": ";
        qoute(attr.value);
        last = false;
        return *this;
    }

    CompactFormat &operator<<(Range attr) {
        std::cout << " @";
        qoute(attr.value);
        last = false;
        return *this;
    }

    CompactFormat &operator<<(EndNodeHeader) {
        inHeader = false;
        std::cout << "\n";
        ++indent;
        return *this;
    }
    CompactFormat &operator<<(EndNode) {
        if (inHeader) {
            std::cout << "\n";
            inHeader = false;
        } else {
            --indent;
        }
        return *this;
    }

    CompactFormat &operator<<(Child name) {
        if (last) {
            indent.stack.back() = 3;
            last = false;
        } else {
            indent.stack.back() = 2;
        }
        savedChildName = name;
        return *this;
    }

    //these two can do nothing if the array is the only child of a node
    CompactFormat &operator<<(BeginArray) {
        std::cout << indent << savedChildName << "\n";
        ++indent;
        return *this;
    }
    CompactFormat &operator<<(EndArray) {
        --indent;
        return *this;
    }

private:
    bool inHeader;
    IndentEx indent;        //TODO merge into this class
    const char *savedChildName;
    bool last{false};

    template<typename T>
    void qoute(T value) {
        std::cout << value;
    }
};

template<>
inline void CompactFormat::qoute(std::string value) {
    //FIXME escape string
    std::cout << "\"" << value << "\"";
}

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_COMPACTFORMAT_H_
