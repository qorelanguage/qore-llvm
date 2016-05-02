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
/// \brief Definition of a basic block.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_BLOCK_H_
#define INCLUDE_QORE_CORE_CODE_BLOCK_H_

#include <memory>
#include <vector>
#include "qore/core/code/Instruction.h"
#include "qore/core/util/Iterators.h"

namespace qore {
namespace code {

/**
 * \brief Represents a basic block.
 *
 * Basic block is an ordered collection of instructions that is executed as a whole (except when an
 * exception is thrown). Only the first instruction in the block can be the target of a jump. The last
 * instruction of a block must be a terminator - a jump, return, throw etc. The only form of flow control allowed in
 * the middle of a block is in the form of landing pads for exceptions.
 */
class Block {

public:
    using Ptr = std::unique_ptr<Block>;                                     //!< Pointer type.
    using Iterator = util::VectorOfUniquePtrIteratorAdapter<Instruction>;   //!< Iterator type.

public:
    /**
     * \brief Default constructor.
     */
    Block() {
    }

    /**
     * \brief Returns an iterator positioned to the first instruction in the block.
     * \return an iterator positioned to the first instruction in the block
     */
    Iterator begin() const {
        return Iterator(instructions.begin());
    }

    /**
     * \brief Returns an iterator positioned past the last instruction of the block.
     * \return an iterator positioned past the last instruction of the block
     */
    Iterator end() const {
        return Iterator(instructions.end());
    }

    /**
     * \brief Appends an instruction to the end of the block.
     * \param instruction the instruction to append
     */
    void append(Instruction::Ptr instruction) {
        assert(instructions.empty() || !instructions.back()->isTerminator());
        instructions.push_back(std::move(instruction));
    }

private:
    Block(const Block &) = delete;
    Block(Block &&) = delete;
    Block &operator=(const Block &) = delete;
    Block &operator=(Block &&) = delete;

private:
    std::vector<Instruction::Ptr> instructions;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_BLOCK_H_
