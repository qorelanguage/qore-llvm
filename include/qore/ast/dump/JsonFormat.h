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
/// \brief Formatter for producing JSON dumps of the AST.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_DUMP_JSONFORMAT_H_
#define INCLUDE_QORE_AST_DUMP_JSONFORMAT_H_

#include <iostream>
#include <vector>
#include "qore/common/Indent.h"
#include "qore/ast/dump/Tags.h"

namespace qore {
namespace ast {
namespace dump {

/**
 * \brief Formatter for producing JSON dumps of the AST.
 * \todo escape strings
 */
class JsonFormat {

public:
    /**
     * Constructor.
     * \param os the destination output stream
     */
    JsonFormat(std::ostream &os = std::cout) : os(os), last(true) {
    }

    /// \name Operator implementation.
    /// \{
    JsonFormat &operator<<(BeginNode beginNode) {
        stack.push_back(last);
        last = false;
        os << "{" << "\n";
        ++indent;
        if (beginNode.type) {
            *this << Attribute<const char *>("node", beginNode.type);
        }
        return *this;
    }

    template<typename T>
    JsonFormat &operator<<(Attribute<T> attr) {
        os << indent << "\"" << attr.name << "\": ";
        qoute(attr.value);
        os << (last ? "" : ",") << "\n";
        last = false;
        return *this;
    }

    JsonFormat &operator<<(EndNodeHeader) {
        return *this;
    }

    JsonFormat &operator<<(EndNode) {
        os << --indent << "}" << (stack.back() ? "" : ",") << "\n";
        stack.pop_back();
        return *this;
    }

    JsonFormat &operator<<(BeginArray) {
        stack.push_back(last);
        last = false;
        os << "[\n";
        ++indent;
        return *this;
    }

    JsonFormat &operator<<(EndArray) {
        os << --indent << "]" << (stack.back() ? "" : ",") << "\n";
        stack.pop_back();
        return *this;
    }

    JsonFormat &operator<<(Child child) {
        os << indent;
        if (child.name) {
            os << "\"" << child.name << "\": ";
        }
        return *this;
    }

    JsonFormat &operator<<(Last) {
        last = true;
        return *this;
    }
    /// \}

private:
    template<typename T>
    void qoute(T value) {
        os << "\"" << value << "\"";
    }

private:
    std::ostream &os;
    std::vector<bool> stack;
    Indent indent;
    bool last;
};

/**
 * \private
 */
template<>
inline void JsonFormat::qoute(uint64_t value) {
    os << value;
}

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_JSONFORMAT_H_
