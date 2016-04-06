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
/// \brief LocalVariable definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IR_DECL_LOCALVARIABLE_H_
#define INCLUDE_QORE_IR_DECL_LOCALVARIABLE_H_

#include <memory>
#include <string>
#include "qore/ir/Type.h"

namespace qore {
namespace ir {

class LocalVariable {

public:
    using Ptr = std::unique_ptr<LocalVariable>;

public:
    static Ptr create(std::string name, const Type &type)  {
        return Ptr(new LocalVariable(std::move(name), type));
    }

public:
    const std::string &getName() const {
        return name;
    }

    const Type &getType() const {
        return type;
    }

private:
    LocalVariable(std::string name, const Type &type) : name(std::move(name)), type(type) {
    }

private:
    LocalVariable(const LocalVariable &) = delete;
    LocalVariable(LocalVariable &&) = delete;
    LocalVariable &operator=(const LocalVariable &) = delete;
    LocalVariable &operator=(LocalVariable &&) = delete;

private:
    std::string name;
    const Type &type;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_DECL_LOCALVARIABLE_H_
