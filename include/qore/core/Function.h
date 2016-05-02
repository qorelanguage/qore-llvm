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
/// \brief Runtime representation of a function.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_FUNCTION_H_
#define INCLUDE_QORE_CORE_FUNCTION_H_

#include <cassert>
#include <string>
#include <vector>
#include "qore/core/FunctionType.h"
#include "qore/core/LocalVariable.h"
#include "qore/core/code/Block.h"

namespace qore {

/**
 * \brief Runtime representation of a function.
 */
class Function {

public:
    using Ptr = std::unique_ptr<Function>;          //!< Pointer type.

public:
    /**
     * \brief Creates the function.
     * \param type the type of the function
     */
    explicit Function(FunctionType type) : type(std::move(type)), tempCount(0) {
    }

    /**
     * \brief Returns the type of the function.
     * \return the type of the function
     */
    const FunctionType &getType() const {
        return type;
    }

    //FIXME
    Size getTempCount() const {
        return tempCount;
    }

    Size getLocalCount() const {
        return locals.size();
    }

    code::Block &getEntryBlock() const {
        assert(!blocks.empty());
        return *blocks[0];
    }

    void setBody(Size tempCount, std::vector<LocalVariable::Ptr> locals, std::vector<code::Block::Ptr> blocks) {
        this->tempCount = tempCount;
        this->locals = std::move(locals);
        this->blocks = std::move(blocks);
    }

//    LocalVariable &addLocalVariable(std::string name, const Type &type) {
//        LocalVariable::Ptr ptr = LocalVariable::Ptr(new LocalVariable(locals.size(), std::move(name), type));
//        LocalVariable &lv = *ptr;
//        locals.push_back(std::move(ptr));
//        return lv;
//    }

private:
    Function(const Function &) = delete;
    Function(Function &&) = delete;
    Function &operator=(const Function &) = delete;
    Function &operator=(Function &&) = delete;

private:
    FunctionType type;

    Size tempCount;
    std::vector<LocalVariable::Ptr> locals;
    std::vector<code::Block::Ptr> blocks;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_FUNCTION_H_
