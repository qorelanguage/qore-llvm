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
/// \brief QIL code representation.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_CODE_H_
#define INCLUDE_QORE_QIL_CODE_H_

#include <vector>
#include "qore/qil/Instruction.h"

namespace qore {
namespace qil {

/**
 * \brief Represents a piece of QIL code.
 */
class Code {

public:
    /**
     * \brief Iterator type.
     */
    using const_iterator = std::vector<Instruction>::const_iterator;

    /**
     * \brief Constructor.
     * \param instructions the instructions making up the piece of code represented by this instance
     */
    Code(std::vector<Instruction> instructions) : instructions(std::move(instructions)) {
    }

    /// \name Default move constructor and assignment.
    /// \{
    Code(Code &&) = default;
    Code &operator=(Code &&) = default;
    /// \}

    /**
     * \brief Returns an iterator positioned at the first instruction.
     * @return iterator positioned at the first instruction
     */
    const_iterator begin() const {
        return instructions.begin();
    }

    /**
     * \brief Returns an iterator positioned beyond the last instruction.
     * @return iterator positioned beyond the last instruction
     */
    const_iterator end() const {
        return instructions.end();
    }

private:
    Code(const Code &) = delete;
    Code &operator=(const Code &) = delete;

private:
    std::vector<Instruction> instructions;
};

/**
 * \brief Dumps the code to an output stream.
 * \param os the output stream
 * \param code the code
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const Code &code) {
    for (const Instruction &i : code) {
        os << i << "\n";
    }
    return os;
}

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_CODE_H_
