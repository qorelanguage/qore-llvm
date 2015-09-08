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
#ifndef INCLUDE_QORE_AST_DUMP_JSONFORMAT_H_
#define INCLUDE_QORE_AST_DUMP_JSONFORMAT_H_

#include <iostream>
#include <vector>
#include "qore/common/Indent.h"
#include "qore/ast/dump/Tags.h"

namespace qore {
namespace ast {
namespace dump {

class JsonFormat {

public:
    JsonFormat &operator<<(Last) {
        last = true;
        return *this;
    }

    JsonFormat &operator<<(BeginNode type) {
        stack.push_back(last);
        last = false;
        std::cout << "{" << "\n";
        ++indent;
        if (type) {
            *this << Attribute<const char *>("node", type);
        }
        return *this;
    }

    template<typename T>
    JsonFormat &operator<<(Attribute<T> attr) {
        std::cout << indent << "\"" << attr.name << "\": ";
        qoute(attr.value);
        std::cout << (last ? "" : ",") << "\n";
        last = false;
        return *this;
    }

    JsonFormat &operator<<(EndNodeHeader) {
        return *this;
    }

    JsonFormat &operator<<(EndNode) {
        std::cout << --indent << "}" << (stack.back() ? "" : ",") << "\n";
        stack.pop_back();
        return *this;
    }

    JsonFormat &operator<<(Child name) {
        std::cout << indent;
        if (name) {
            std::cout << "\"" << name << "\": ";
        }
        return *this;
    }

    JsonFormat &operator<<(BeginArray) {
        stack.push_back(last);
        last = false;
        std::cout << "[\n";
        ++indent;
        return *this;
    }

    JsonFormat &operator<<(EndArray) {
        std::cout << --indent << "]" << (stack.back() ? "" : ",") << "\n";
        stack.pop_back();
        return *this;
    }

private:
    Indent indent;
    std::vector<bool> stack;
    bool last{true};

    template<typename T>
    void qoute(T value) {
        //FIXME escape string
        std::cout << "\"" << value << "\"";
    }
};

template<>
inline void JsonFormat::qoute(uint64_t value) {
    std::cout << value;
}

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_JSONFORMAT_H_
