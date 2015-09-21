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

#include "qore/qil/Instruction.h"

namespace qore {
namespace qil {

//TODO make immutable, cleanup interface and document

/**
 * \brief Represents a piece of QIL code.
 */
class Code {

public:
    Code() {
    }

    Code(Code &&) = default;
    Code &operator=(Code &&) = default;

    std::vector<Instruction>::const_iterator begin() const {
        return code.begin();
    }

    std::vector<Instruction>::const_iterator end() const {
        return code.end();
    }

public:
    void add(Instruction ins) {
        code.emplace_back(std::move(ins));
    }

private:
    std::vector<Instruction> code;
    Code(const Code &) = delete;
    Code &operator=(const Code &) = delete;

    friend std::ostream &operator<<(std::ostream &os, const Code &c) {
        for (const Instruction &i : c.code) {
            os << i << "\n";
        }
        return os;
    }
};

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_CODE_H_
