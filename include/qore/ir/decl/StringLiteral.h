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
/// \brief StringLiteral definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IR_DECL_STRINGLITERAL_H_
#define INCLUDE_QORE_IR_DECL_STRINGLITERAL_H_

#include <memory>
#include <string>

namespace qore {
namespace ir {

class StringLiteral {

public:
    using Ptr = std::unique_ptr<StringLiteral>;

public:
    static Ptr create(std::string value) {
        return Ptr(new StringLiteral(std::move(value)));
    }

    const std::string &getValue() const {
        return value;
    }

private:
    explicit StringLiteral(std::string value) : value(std::move(value)) {
    }

private:
    StringLiteral(const StringLiteral &) = delete;
    StringLiteral(StringLiteral &&) = delete;
    StringLiteral &operator=(const StringLiteral &) = delete;
    StringLiteral &operator=(StringLiteral &&) = delete;

private:
    std::string value;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_DECL_STRINGLITERAL_H_
