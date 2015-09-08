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
#ifndef INCLUDE_QORE_AST_DUMP_YAMLFORMAT_H_
#define INCLUDE_QORE_AST_DUMP_YAMLFORMAT_H_

#include <iostream>
#include <string>
#include "qore/common/Indent.h"
#include "qore/ast/dump/Tags.h"

namespace qore {
namespace ast {
namespace dump {

//FIXME escape strings

struct I : public Indent {

    I() : Indent(-1) {
    }

    std::string get() {
        std::string s = Indent::get();
        if (spec) {
            spec = false;
            s[s.size() - Multiplier] = '-';
        }
        return s;
    }

    bool spec{false};
};

inline std::ostream &operator<<(std::ostream &os, I &indent) {
    return os << indent.get();
}

class YamlFormat {

public:
    template<typename T>
    YamlFormat &operator<<(T) {
        return *this;
    }

    YamlFormat &operator<<(BeginNode type) {
        ++indent;
        if (type) {
            *this << Attribute<const char *>("node", type);
        }
        return *this;
    }

    template<typename T>
    YamlFormat &operator<<(Attribute<T> attr) {
        std::cout << indent << attr.name << ": " << attr.value << "\n";
        return *this;
    }

    YamlFormat &operator<<(EndNode) {
        --indent;
        return *this;
    }

    YamlFormat &operator<<(Child name) {
        if (name) {
            std::cout << indent << name << ":\n";
        } else {
            indent.spec = true;
        }
        return *this;
    }

private:
    I indent;
};

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_YAMLFORMAT_H_
