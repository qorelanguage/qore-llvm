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
#include "qore/core/util/Iterators.h"

namespace qore {

class FunctionGroup;

/**
 * \brief Runtime representation of a function.
 */
class Function {

public:
    using Ptr = std::unique_ptr<Function>;                                          //!< Pointer type.
    using LocalVariableIterator = util::VectorOfPtrIteratorAdapter<LocalVariable>;  //!< Locals iterator.

public:
    /**
     * \brief Creates the function.
     * \param group the parent function group
     * \param type the type of the function
     * \param location the location of the declaration
     */
    Function(const FunctionGroup &group, FunctionType type, SourceLocation location)
            : group(group), type(std::move(type)), location(location), tempCount(0) {
    }

    /**
     * \brief Returns the parent function group.
     * \return the parent function group
     */
    const FunctionGroup &getGroup() const {
        return group;
    }

    /**
     * \brief Returns the type of the function.
     * \return the type of the function
     */
    const FunctionType &getType() const {
        return type;
    }

    /**
     * \brief Returns the location of the declaration.
     * \return the location of the declaration
     */
    const SourceLocation &getLocation() const {
        return location;
    }

    /**
     * \brief Returns the number of temporaries needed for interpreting this functions.
     *
     * It is guaranteed that every \ref code::Temp::getIndex() in the function's code is less than this value.
     * \return the number of temporaries needed for interpreting this functions
     */
    Size getTempCount() const {
        return tempCount;
    }

    /**
     * \brief Returns the block with the entry point of the function.
     * \return the block with the entry point of the function
     */
    code::Block &getEntryBlock() const {
        assert(!blocks.empty());
        return *blocks[0];
    }

    /**
     * \brief Adds a new temporary by increasing the number of temporaries by one.
     * \return the new temporary value
     */
    code::Temp addTemp() {
        return code::Temp(tempCount++);
    }

    /**
     * \brief Adds a new local variable.
     * \param name the name of the variable
     * \param type the type of the variable
     * \param location the location of the declaration
     * \return the new local variable
     */
    LocalVariable &addLocalVariable(std::string name, const Type &type, SourceLocation location) {
        LocalVariable::Ptr ptr = LocalVariable::Ptr(new LocalVariable(locals.size(), std::move(name), type, location));
        LocalVariable &lv = *ptr;
        locals.push_back(std::move(ptr));
        return lv;
    }

    /**
     * \brief Adds a new block to the function.
     *
     * The first block created serves as the entry point to the function.
     * \return the new block
     */
    code::Block *addBlock() {
        code::Block::Ptr ptr = code::Block::Ptr(new code::Block());
        code::Block *b = ptr.get();
        blocks.push_back(std::move(ptr));
        return b;
    }

    /**
     * \brief Returns a range for iterating local variables.
     * \return a range for iterating local variables
     */
    util::IteratorRange<LocalVariableIterator> getLocalVariables() const {
        return util::IteratorRange<LocalVariableIterator>(locals);
    }

    /**
     * \brief Returns false if it is guaranteed that the function cannot throw an exception.
     * \return false if it is guaranteed that the function cannot throw an exception
     */
    bool canThrow() const {
        //TODO Function::canThrow()
        return true;
    }

private:
    Function(const Function &) = delete;
    Function(Function &&) = delete;
    Function &operator=(const Function &) = delete;
    Function &operator=(Function &&) = delete;

private:
    const FunctionGroup &group;
    FunctionType type;
    SourceLocation location;

    Size tempCount;
    std::vector<LocalVariable::Ptr> locals;
    std::vector<code::Block::Ptr> blocks;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_FUNCTION_H_
