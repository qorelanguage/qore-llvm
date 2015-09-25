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
/// \brief Formatter for producing XML dumps of the AST.
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

/**
 * \brief Formatter for producing XML dumps of the AST.
 * \todo escape strings
 */
class XmlFormat {

public:
    /**
     * Constructor.
     * \param os the destination output stream
     */
    XmlFormat(std::ostream &os = std::cout) : os(os), inHeader(false), savedChildName(nullptr) {
    }

    /// \name Operator implementation.
    /// \{
    XmlFormat &operator<<(BeginNode beginNode) {
        const char *tag = beginNode.type ? beginNode.type : savedChildName;
        os << indent << "<" << tag;
        stack.push(tag);
        inHeader = true;
        return *this;
    }

    template<typename T>
    XmlFormat &operator<<(Attribute<T> attr) {
        os << " " << attr.name << "=\"" << attr.value << "\"";
        return *this;
    }

    XmlFormat &operator<<(EndNodeHeader) {
        inHeader = false;
        os << ">\n";
        ++indent;
        return *this;
    }

    XmlFormat &operator<<(EndNode) {
        if (inHeader) {
            os << " />\n";
            inHeader = false;
        } else {
            os << --indent << "</" << stack.top() << ">\n";
        }
        stack.pop();
        return *this;
    }

    XmlFormat &operator<<(BeginArray) {
        os << indent++ << "<" << savedChildName << ">\n";
        stack.push(savedChildName);
        return *this;
    }

    XmlFormat &operator<<(EndArray) {
        os << --indent << "</" << stack.top() << ">\n";
        stack.pop();
        return *this;
    }

    XmlFormat &operator<<(Child child) {
        savedChildName = child.name;
        return *this;
    }

    XmlFormat &operator<<(Last) {
        return *this;
    }
    /// \}

private:
    std::ostream &os;
    std::stack<std::string> stack;
    Indent indent;
    bool inHeader;
    const char *savedChildName;
};

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_XMLFORMAT_H_
