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
#ifndef INCLUDE_QORE_COMP_AS_FUNCTION_H_
#define INCLUDE_QORE_COMP_AS_FUNCTION_H_

#include <cassert>
#include <string>
#include <vector>
#include "qore/comp/as/Block.h"
#include "qore/comp/as/LocalVariable.h"
#include "qore/core/Type.h"

namespace qore {
namespace comp {
namespace as {

class Function {

public:
    Function(std::string name, Id argCount, const Type &retType, Id tempCount,
            std::vector<LocalVariable::Ptr> locals, std::vector<Block::Ptr> blocks)
            : name(std::move(name)), argCount(argCount), retType(retType),
            tempCount(tempCount), locals(std::move(locals)), blocks(std::move(blocks)) {
    }

    const std::string &getName() const {
        return name;
    }

    Id getArgCount() const {
        return argCount;
    }

    Id getTempCount() const {
        return tempCount;
    }

    Id getLocalCount() const {
        return locals.size();
    }

    Block &getEntryBlock() const {
        assert(!blocks.empty());
        return *blocks[0];
    }

private:
    std::string name;
    Id argCount;
    const Type &retType;
    Id tempCount;
    std::vector<LocalVariable::Ptr> locals;
    std::vector<Block::Ptr> blocks;
};

} // namespace as
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AS_FUNCTION_H_
