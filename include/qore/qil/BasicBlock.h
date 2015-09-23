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
/// \brief TODO File description
///
//------------------------------------------------------------------------------

#ifndef INCLUDE_QORE_QIL_BASICBLOCK_H_
#define INCLUDE_QORE_QIL_BASICBLOCK_H_

#include <vector>
#include "qore/qil/Instruction.h"

namespace qore {
namespace qil {

class BasicBlock;

class RetVoidTerminator;
class ConditionalTerminator;
class UnconditionalTerminator;

class TerminatorVisitor {
public:
    virtual ~TerminatorVisitor() = default;
    virtual void visit(const RetVoidTerminator *) = 0;
    virtual void visit(const ConditionalTerminator *) = 0;
    virtual void visit(const UnconditionalTerminator *) = 0;
};

class Terminator {

public:
    virtual ~Terminator() = default;
    virtual void accept(TerminatorVisitor &) = 0;
};

class RetVoidTerminator : public Terminator {
public:
    RetVoidTerminator() {
    }

    void accept(TerminatorVisitor &visitor) override {
        visitor.visit(this);
    }
};

class ConditionalTerminator : public Terminator {
public:
    BasicBlock *thenBlock;
    BasicBlock *elseBlock;

    ConditionalTerminator(BasicBlock *thenBlock, BasicBlock *elseBlock) : thenBlock(thenBlock), elseBlock(elseBlock) {
    }

    void accept(TerminatorVisitor &visitor) override {
        visitor.visit(this);
    }
};

class UnconditionalTerminator : public Terminator {
public:
    BasicBlock *nextBlock;

    UnconditionalTerminator(BasicBlock *nextBlock) : nextBlock(nextBlock) {
    }

    void accept(TerminatorVisitor &visitor) override {
        visitor.visit(this);
    }
};








/**
 * \brief Represents a basic block.
 */
class BasicBlock {

public:
    using Data = void *;

public:
    std::vector<Instruction> instructions;
    std::unique_ptr<Terminator> terminator;
    Data data;

public:
    BasicBlock() : data(nullptr) {
    }

private:
    BasicBlock(const BasicBlock &) = delete;
    BasicBlock(BasicBlock &&) = delete;
    BasicBlock &operator=(const BasicBlock &) = delete;
    BasicBlock &operator=(BasicBlock &&) = delete;
};

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_BASICBLOCK_H_
