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
#ifndef INCLUDE_QORE_ANALYZER_EXPR_CODE_H_
#define INCLUDE_QORE_ANALYZER_EXPR_CODE_H_

namespace qore {
namespace analyzer {

enum class Opcode {
    LoadLocVarPtr,
    PushString,
    Swap,
    CleanupLValue,
    PopAndDeref,
    PushLocVar,
    LoadUnique,
    Trim,
    Dup,
    Add,
    PushNothing,
    Print,
    LifetimeStart,
    LifetimeEnd,
    Ret,
};

class Ins {
public:
    Ins(Opcode opcode) : opcode(opcode), locVar(nullptr) {
    }

    Ins(Opcode opcode, LocalVariable *locVar) : opcode(opcode), locVar(locVar) {
    }

    Ins(Opcode opcode, StringLiteral *strLit) : opcode(opcode), strLit(strLit) {
    }

    Opcode getOpcode() const {
        return opcode;
    }

    const LocalVariable *getLocVar() const {
        assert(opcode == Opcode::LoadLocVarPtr || opcode == Opcode::PushLocVar || opcode == Opcode::LifetimeStart || opcode == Opcode::LifetimeEnd);
        return locVar;
    }

    const StringLiteral *getStrLit() const {
        assert(opcode == Opcode::PushString);
        return strLit;
    }

private:
    Opcode opcode;
    union {
        LocalVariable *locVar;
        StringLiteral *strLit;
    };

    friend std::ostream &operator<<(std::ostream &os, const Ins &i) {
        switch (i.opcode) {
            case Opcode::LoadLocVarPtr:
                return os << "LOAD_PTR of " << *i.locVar;
            case Opcode::PushLocVar:
                return os << "PUSH " << *i.locVar;
            case Opcode::PushString:
                return os << "PUSH " << *i.strLit;
            case Opcode::Swap:
                return os << "SWAP";
            case Opcode::CleanupLValue:
                return os << "CLEANUP_LVALUE";
            case Opcode::PopAndDeref:
                return os << "POP_AND_DEREF";
            case Opcode::LoadUnique:
                return os << "LOAD_UNIQUE";
            case Opcode::Trim:
                return os << "TRIM";
            case Opcode::Dup:
                return os << "DUP";
            case Opcode::Add:
                return os << "ADD";
            case Opcode::PushNothing:
                return os << "PUSH_NOTHING";
            case Opcode::Print:
                return os << "PRINT";
            case Opcode::LifetimeStart:
                return os << "LIFETIME_START " << *i.locVar;
            case Opcode::LifetimeEnd:
                return os << "LIFETIME_END " << *i.locVar;
            case Opcode::Ret:
                return os << "RET";
        }
        return os << "???";
    }
};

class Code {

public:
    Code() {
    }

    Code(Code &&) = default;
    Code &operator=(Code &&) = default;

    std::vector<Ins>::const_iterator begin() const {
        return code.begin();
    }

    std::vector<Ins>::const_iterator end() const {
        return code.end();
    }

public:
    void add(Ins ins) {                     //make code immutable?
        code.emplace_back(std::move(ins));
    }

private:
    std::vector<Ins> code;
    Code(const Code &) = delete;
    Code &operator=(const Code &) = delete;

    friend std::ostream &operator<<(std::ostream &os, const Code &c) {
        for (const Ins &i : c.code) {
            os << i << "\n";
        }
        return os;
    }
};


} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_EXPR_CODE_H_
