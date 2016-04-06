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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_SYMBOL_H_
#define INCLUDE_QORE_COMP_SEM_SYMBOL_H_

#include <cassert>
#include "qore/ir/decl/GlobalVariable.h"
#include "qore/ir/decl/LocalVariable.h"

namespace qore {
namespace comp {
namespace sem {

class Symbol {

public:
    enum class Kind {
        None,
        Global,
        Local,
    };

public:
    Symbol() : kind(Kind::None), ptr(nullptr) {
    }

    explicit Symbol(const ir::LocalVariable &v) : kind(Kind::Local), ptr(&v) {
    }

    explicit Symbol(const ir::GlobalVariable &v) : kind(Kind::Global), ptr(&v) {
    }

    Kind getKind() const {
        return kind;
    }

    const ir::LocalVariable &asLocal() const {
        assert(kind == Kind::Local);
        return *static_cast<const ir::LocalVariable *>(ptr);
    }

    const ir::GlobalVariable &asGlobal() const {
        assert(kind == Kind::Global);
        return *static_cast<const ir::GlobalVariable *>(ptr);
    }

private:
    Kind kind;
    const void *ptr;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_SYMBOL_H_
