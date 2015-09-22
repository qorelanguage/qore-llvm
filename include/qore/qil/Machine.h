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
/// \brief Implementation of the QIL Stack Machine.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_MACHINE_H_
#define INCLUDE_QORE_QIL_MACHINE_H_

namespace qore {
namespace qil {

/**
 * \brief Implementation of the QIL stack machine.
 * \see qore::qil
 */
template<typename Backend>
class Machine {

public:
    /**
     * \brief Constructor.
     * \param backend the back-end to use for actual execution
     */
    Machine(Backend &backend) : backend(backend), lValReg(nullptr) {
    }

    /**
     * \brief Executes give QIL code.
     * \param code the code to execute
     */
    void run(const Code &code) {
        for (const auto &ins : code) {
            LOG("EXECUTING: " << ins);
            execute(ins);
        }
    }

    /**
     * \brief Executes a single instruction.
     * \param ins the instruction to execute
     */
    void execute(const Instruction &ins) {
        backend.setLocation(ins.location);
        switch (ins.opcode) {
            case Opcode::Add:
                add();
                break;
            case Opcode::Assign:
                assign();
                break;
            case Opcode::Discard:
                discard();
                break;
            case Opcode::Dup:
                dup();
                break;
            case Opcode::LifetimeEnd:
                lifetimeEnd(ins.variable);
                break;
            case Opcode::LifetimeStart:
                lifetimeStart(ins.variable);
                break;
            case Opcode::LoadUnique:
                loadUnique();
                break;
            case Opcode::LoadVarPtr:
                loadVarPtr(ins.variable);
                break;
            case Opcode::Print:
                print();
                break;
            case Opcode::PushNothing:
                pushNothing();
                break;
            case Opcode::PushString:
                pushString(ins.stringLiteral);
                break;
            case Opcode::PushVar:
                pushVar(ins.variable);
                break;
            case Opcode::Trim:
                trim();
                break;
            default:
                QORE_UNREACHABLE("Not implemented " << ins);
        }
    }

    /**
     * \brief Logs the current state of the machine.
     */
    void dump() {
        LOG_FUNCTION();
        LOG("STACK:" << (stack.empty() ? " <empty>" : ""));
        for (const auto &v : stack) {
            LOG("  " << v);
        }
        LOG("LVALREG: " << lValReg);
        LOG("DISCARD QUEUE:" << (discardQueue.empty() ? " <empty>" : ""));
        for (const auto &v : discardQueue) {
            LOG("  " << v);
        }
    }

private:
    using Value = typename Backend::Value;
    using LValue = typename Backend::LValue;
    using VariableData = typename Backend::VariableData;
    using StringLiteralData = typename Backend::StringLiteralData;

private:
    void add() {
        assert(stack.size() >= 2 && "ADD requires two operands");
        Value r = stack.back();
        stack.pop_back();
        Value l = stack.back();
        stack.back() = backend.add(l, r);
        discard(r);
        discard(l);
    }

    void assign() {
        assert(lValReg && "ASSIGN requires an l-value");
        assert(!stack.empty() && "ASSIGN requires an operand");
        const Value &newValue = stack.back();
        discard(backend.assign(lValReg, newValue));
        stack.pop_back();
        lValReg = nullptr;
        emptyDiscardQueue();
    }

    void discard() {
        assert(!stack.empty() && "DISCARD requires an operand");
        discard(stack.back());
        stack.pop_back();
    }

    void dup() {
        assert(!stack.empty() && "DUP requires an operand");
        const Value &v = stack.back();
        backend.strongRef(v);
        stack.push_back(v);
    }

    void lifetimeEnd(const Variable *variable) {
        discard(backend.loadVariableValue(static_cast<VariableData>(variable->data)));
        backend.lifetimeEnd(static_cast<VariableData>(variable->data));
    }

    void lifetimeStart(const Variable *variable) {
        backend.lifetimeStart(static_cast<VariableData>(variable->data));
    }

    void loadUnique() {
        assert(lValReg && "LOAD_UNIQUE requires an l-value");
        stack.push_back(backend.loadUnique(lValReg));
    }

    void loadVarPtr(const Variable *variable) {
        assert(!lValReg && "LOAD_VAR_PTR requires empty LVALREG");
        lValReg = backend.loadVarPtr(static_cast<VariableData>(variable->data));
    }

    void print() {
        Value v = stack.back();
        stack.pop_back();
        backend.print(v);
        discard(v);
    }

    void pushNothing() {
        stack.push_back(backend.getNothing());
    }

    void pushString(const StringLiteral *stringLiteral) {
        Value v = backend.loadStringLiteralValue(static_cast<StringLiteralData>(stringLiteral->data));
        backend.strongRef(v);
        stack.push_back(v);
    }

    void pushVar(const Variable *variable) {
        Value v = backend.loadVariableValue(static_cast<VariableData>(variable->data));
        backend.strongRef(v);
        stack.push_back(v);
    }

    void trim() {
        assert(!stack.empty() && "TRIM requires an operand");
        const Value &v = stack.back();
        backend.trim(v);
    }

    void discard(const Value &v) {
        if (lValReg) {
            discardQueue.push_back(v);
        } else {
            backend.strongDeref(v);
        }
    }

    void emptyDiscardQueue() {
        for (const Value &v : discardQueue) {
            backend.strongDeref(v);
        }
        discardQueue.clear();
    }

private:
    Backend &backend;
    std::deque<Value> stack;
    std::vector<Value> discardQueue;
    LValue lValReg;
};

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_MACHINE_H_
