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
#ifndef INCLUDE_QORE_AST_DUMP_XMLFORMAT_H_
#define INCLUDE_QORE_AST_DUMP_XMLFORMAT_H_

#include <iostream>
#include <stack>
#include <string>
#include "qore/common/Indent.h"
#include "qore/ast/dump/Tags.h"

namespace qore {
namespace ast {
namespace dump {

class XmlFormat {

public:
    XmlFormat &operator<<(Last) {
        return *this;
    }

    XmlFormat &operator<<(BeginNode type) {
        const char *tag = type || savedChildName;
        std::cout << indent << "<" << tag;
        stack.push(tag);
        inHeader = true;
        return *this;
    }

    template<typename T>
    XmlFormat &operator<<(Attribute<T> attr) {
        std::cout << " " << attr.name << "=\"" << attr.value << "\"";
        return *this;
    }

    XmlFormat &operator<<(EndNodeHeader) {
        inHeader = false;
        std::cout << ">\n";
        ++indent;
        return *this;
    }

    XmlFormat &operator<<(EndNode) {
        if (inHeader) {
            std::cout << " />\n";
            inHeader = false;
        } else {
            std::cout << --indent << "</" << stack.top() << ">\n";
        }
        stack.pop();
        return *this;
    }

    XmlFormat &operator<<(Child name) {
        savedChildName = name;
        return *this;
    }

    //these two can do nothing if the array is the only child of a node
    XmlFormat &operator<<(BeginArray) {
        std::cout << indent++ << "<" << savedChildName << ">\n";
        stack.push(savedChildName);
        return *this;
    }

    XmlFormat &operator<<(EndArray) {
        std::cout << --indent << "</" << stack.top() << ">\n";
        stack.pop();
        return *this;
    }

private:
    bool inHeader;
    Indent indent;
    const char *savedChildName;
    std::stack<std::string> stack;
};
//FIXME escape strings
//TODO std::cout -> ostream
} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_XMLFORMAT_H_
