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
/// \brief Defines the Jump instruction.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CODE_JUMP_H_
#define INCLUDE_QORE_CORE_CODE_JUMP_H_

#include "qore/core/code/Instruction.h"

namespace qore {
namespace code {

/**
 * \brief A terminator instruction that jumps to another block.
 */
class Jump : public Instruction {

public:
    /**
     * \brief Constructor.
     * \param dest the block to jump to
     */
    explicit Jump(const Block &dest) : dest(dest) {
    }

    Kind getKind() const override {
        return Kind::Jump;
    }

    /**
     * \brief Returns the block to jump to.
     * \return the block to jump to
     */
    const Block &getDest() const {
        return dest;
    }

private:
    const Block &dest;
};

} // namespace code
} // namespace qore

#endif // INCLUDE_QORE_CORE_CODE_JUMP_H_
