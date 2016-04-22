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
#ifndef INCLUDE_QORE_COMP_AS_IS_H_
#define INCLUDE_QORE_COMP_AS_IS_H_

#include <string>
#include "qore/Id.h"
#include "qore/rt/Types.h"
#include "qore/rt/Meta.h"
#include "qore/comp/as/Instruction.h"
#include "qore/comp/as/Block.h"
#include "qore/comp/as/StringLiteral.h"
#include "qore/comp/as/Temp.h"
#include "qore/comp/as/LocalSlot.h"
#include "qore/comp/as/GlobalVariable.h"

namespace qore {
namespace comp {
namespace as {

class IntConstant : public Instruction {

public:
    IntConstant(Temp dest, rt::qint value) : dest(dest), value(value) {
    }

    Kind getKind() const override {
        return Kind::IntConstant;
    }

    Temp getDest() const {
        return dest;
    }

    rt::qint getValue() const {
        return value;
    }

private:
    Temp dest;
    rt::qint value;
};

class GetLocal : public Instruction {

public:
    GetLocal(Temp dest, LocalSlot slot) : dest(dest), slot(slot) {
    }

    Kind getKind() const override {
        return Kind::GetLocal;
    }

    Temp getDest() const {
        return dest;
    }

    LocalSlot getSlot() const {
        return slot;
    }

private:
    Temp dest;
    LocalSlot slot;
};

class GetArg : public Instruction {

public:
    GetArg(Temp dest, Id index) : dest(dest), index(index) {
    }

    Kind getKind() const override {
        return Kind::GetArg;
    }

    Temp getDest() const {
        return dest;
    }

    Id getIndex() const {
        return index;
    }

private:
    Temp dest;
    Id index;
};


class SetLocal : public Instruction {

public:
    SetLocal(LocalSlot slot, Temp src) : slot(slot), src(src) {
    }

    Kind getKind() const override {
        return Kind::SetLocal;
    }

    LocalSlot getSlot() const {
        return slot;
    }

    Temp getSrc() const {
        return src;
    }

private:
    LocalSlot slot;
    Temp src;
};

class LoadString : public Instruction {

public:
    LoadString(Temp dest, StringLiteral stringLiteral) : dest(dest), stringLiteral(stringLiteral) {
    }

    Kind getKind() const override {
        return Kind::LoadString;
    }

    Temp getDest() const {
        return dest;
    }

    StringLiteral getStringLiteral() const {
        return stringLiteral;
    }

private:
    Temp dest;
    StringLiteral stringLiteral;
};

class RefInc : public Instruction {

public:
    explicit RefInc(Temp temp) : temp(temp) {
    }

    Kind getKind() const override {
        return Kind::RefInc;
    }

    Temp getTemp() const {
        return temp;
    }

private:
    Temp temp;
};

class RefDec : public Instruction {

public:
    RefDec(Temp temp, Block *lpad) : Instruction(lpad), temp(temp) {
    }

    Kind getKind() const override {
        return Kind::RefDec;
    }

    Temp getTemp() const {
        return temp;
    }

private:
    Temp temp;
};

class RefDecNoexcept : public Instruction {

public:
    RefDecNoexcept(Temp temp) : temp(temp) {
    }

    Kind getKind() const override {
        return Kind::RefDecNoexcept;
    }

    Temp getTemp() const {
        return temp;
    }

private:
    Temp temp;
};

class ReadLockGlobal : public Instruction {

public:
    explicit ReadLockGlobal(const GlobalVariable &globalVariable) : globalVariable(globalVariable) {
    }

    Kind getKind() const override {
        return Kind::ReadLockGlobal;
    }

    GlobalVariable getGlobalVariable() const {
        return globalVariable;
    }

private:
    const GlobalVariable &globalVariable;
};

class ReadUnlockGlobal : public Instruction {

public:
    explicit ReadUnlockGlobal(const GlobalVariable &globalVariable) : globalVariable(globalVariable) {
    }

    Kind getKind() const override {
        return Kind::ReadUnlockGlobal;
    }

    GlobalVariable getGlobalVariable() const {
        return globalVariable;
    }

private:
    const GlobalVariable &globalVariable;
};

class WriteLockGlobal : public Instruction {

public:
    explicit WriteLockGlobal(const GlobalVariable &globalVariable) : globalVariable(globalVariable) {
    }

    Kind getKind() const override {
        return Kind::WriteLockGlobal;
    }

    GlobalVariable getGlobalVariable() const {
        return globalVariable;
    }

private:
    const GlobalVariable &globalVariable;
};

class WriteUnlockGlobal : public Instruction {

public:
    explicit WriteUnlockGlobal(const GlobalVariable &globalVariable) : globalVariable(globalVariable) {
    }

    Kind getKind() const override {
        return Kind::WriteUnlockGlobal;
    }

    GlobalVariable getGlobalVariable() const {
        return globalVariable;
    }

private:
    const GlobalVariable &globalVariable;
};

class GetGlobal : public Instruction {

public:
    GetGlobal(Temp dest, const GlobalVariable &globalVariable) : dest(dest), globalVariable(globalVariable) {
    }

    Kind getKind() const override {
        return Kind::GetGlobal;
    }

    Temp getDest() const {
        return dest;
    }

    GlobalVariable getGlobalVariable() const {
        return globalVariable;
    }

private:
    Temp dest;
    const GlobalVariable &globalVariable;
};

class SetGlobal: public Instruction {

public:
    SetGlobal(const GlobalVariable &globalVariable, Temp src) : globalVariable(globalVariable), src(src) {
    }

    Kind getKind() const override {
        return Kind::SetGlobal;
    }

    GlobalVariable getGlobalVariable() const {
        return globalVariable;
    }

    Temp getSrc() const {
        return src;
    }

private:
    const GlobalVariable &globalVariable;
    Temp src;
};

class MakeGlobal: public Instruction {

public:
    MakeGlobal(const GlobalVariable &globalVariable, Temp initValue) : globalVariable(globalVariable),
            initValue(initValue) {
    }

    Kind getKind() const override {
        return Kind::MakeGlobal;
    }

    const GlobalVariable &getGlobalVariable() const {
        return globalVariable;
    }

    Temp getInitValue() const {
        return initValue;
    }

private:
    const GlobalVariable &globalVariable;
    Temp initValue;
};

class Rethrow : public Instruction {

public:
    Rethrow() {
    }

    Kind getKind() const override {
        return Kind::Rethrow;
    }
};

class BinaryOperator : public Instruction {

public:
    BinaryOperator(Temp dest, const rt::meta::BinaryOperatorDesc &desc, Temp left, Temp right, Block *lpad)
            : Instruction(lpad), dest(dest), desc(desc), left(left), right(right) {
    }

    Kind getKind() const override {
        return Kind::BinaryOperator;
    }

    Temp getDest() const {
        return dest;
    }

    const rt::meta::BinaryOperatorDesc &getDesc() const {
        return desc;
    }

    Temp getLeft() const {
        return left;
    }

    Temp getRight() const {
        return right;
    }

private:
    Temp dest;
    const rt::meta::BinaryOperatorDesc &desc;
    Temp left;
    Temp right;
};

class Conversion : public Instruction {

public:
    Conversion(Temp dest, const rt::meta::ConversionDesc &desc, Temp arg, Block *lpad)
            : Instruction(lpad), dest(dest), desc(desc), arg(arg) {
    }

    Kind getKind() const override {
        return Kind::Conversion;
    }

    Temp getDest() const {
        return dest;
    }

    const rt::meta::ConversionDesc &getDesc() const {
        return desc;
    }

    Temp getArg() const {
        return arg;
    }

private:
    Temp dest;
    const rt::meta::ConversionDesc &desc;
    Temp arg;
};

class RetVoid : public Instruction {

public:
    RetVoid() {
    }

    Kind getKind() const override {
        return Kind::RetVoid;
    }
};

class Ret : public Instruction {

public:
    explicit Ret(Temp value) : value(value) {
    }

    Kind getKind() const override {
        return Kind::Ret;
    }

    Temp getValue() const {
        return value;
    }

private:
    Temp value;
};

class MakeStringLiteral : public Instruction {

public:
    MakeStringLiteral(StringLiteral stringLiteral, std::string value) : stringLiteral(stringLiteral),
            value(std::move(value)) {
    }

    Kind getKind() const override {
        return Kind::MakeStringLiteral;
    }

    StringLiteral getStringLiteral() const {
        return stringLiteral;
    }

    const std::string getValue() const {
        return value;
    }

private:
    StringLiteral stringLiteral;
    std::string value;
};

class Jump : public Instruction {

public:
    explicit Jump(const Block &dest) : dest(dest) {
    }

    Kind getKind() const override {
        return Kind::Jump;
    }

    const Block &getDest() const {
        return dest;
    }

private:
    const Block &dest;
};

class Branch: public Instruction {

public:
    Branch(Temp condition, const Block &trueDest, const Block &falseDest) : condition(condition), trueDest(trueDest),
            falseDest(falseDest) {
    }

    Kind getKind() const override {
        return Kind::Branch;
    }

    Temp getCondition() const {
        return condition;
    }

    const Block &getTrueDest() const {
        return trueDest;
    }

    const Block &getFalseDest() const {
        return falseDest;
    }

private:
    Temp condition;
    const Block &trueDest;
    const Block &falseDest;
};


//class Dump {
//
//public:
//    Dump() : os(std::cout) {
//    }
//
//    void dump(F &f) {
//        for (auto &b : f.blocks) {
//            dump(*b);
//        }
//    }
//
//    void dump(Block &b) {
//        std::cout << "BB." << b.id << "\n";
//
//        Id i = 0;
//        for (auto &ins : b.instructions) {
//            insIndices[ins.get()] = i++;
//        }
//
//        for (auto &ins : b.instructions) {
//            dump(*ins);
//        }
//        insIndices.clear();
//    }
//
//    void dump(Instruction &ins) {
//        os << std::setw(3) << std::right << insIndices[&ins] << ": ";
//        switch (ins.getKind()) {
//            case Instruction::Kind::IntConstant:
//                dump(static_cast<IntConstant &>(ins));
//                break;
//            case Instruction::Kind::GetLocal:
//                dump(static_cast<GetLocal &>(ins));
//                break;
//            case Instruction::Kind::SetLocal:
//                dump(static_cast<SetLocal &>(ins));
//                break;
//            case Instruction::Kind::LoadString:
//                dump(static_cast<LoadString &>(ins));
//                break;
//            case Instruction::Kind::RefInc:
//                dump(static_cast<RefInc &>(ins));
//                break;
//            case Instruction::Kind::RefDec:
//                dump(static_cast<RefDec &>(ins));
//                break;
//            case Instruction::Kind::RefDecNoexcept:
//                dump(static_cast<RefDecNoexcept &>(ins));
//                break;
//            case Instruction::Kind::ReadLockGlobal:
//                dump(static_cast<ReadLockGlobal &>(ins));
//                break;
//            case Instruction::Kind::ReadUnlockGlobal:
//                dump(static_cast<ReadUnlockGlobal &>(ins));
//                break;
//            case Instruction::Kind::WriteLockGlobal:
//                dump(static_cast<WriteLockGlobal &>(ins));
//                break;
//            case Instruction::Kind::WriteUnlockGlobal:
//                dump(static_cast<WriteUnlockGlobal &>(ins));
//                break;
//            case Instruction::Kind::GetGlobal:
//                dump(static_cast<GetGlobal &>(ins));
//                break;
//            case Instruction::Kind::SetGlobal:
//                dump(static_cast<SetGlobal &>(ins));
//                break;
//            case Instruction::Kind::MakeGlobal:
//                dump(static_cast<MakeGlobal &>(ins));
//                break;
//            case Instruction::Kind::FreeGlobal:
//                dump(static_cast<FreeGlobal &>(ins));
//                break;
//            case Instruction::Kind::LandingPad:
//                dump(static_cast<LandingPad &>(ins));
//                break;
//            case Instruction::Kind::Rethrow:
//                dump(static_cast<Rethrow &>(ins));
//                break;
//            case Instruction::Kind::BinaryOperator:
//                dump(static_cast<BinaryOperator &>(ins));
//                break;
//            case Instruction::Kind::Conversion:
//                dump(static_cast<Conversion &>(ins));
//                break;
//            case Instruction::Kind::RetVoid:
//                dump(static_cast<RetVoid &>(ins));
//                break;
//            default:
//                QORE_NOT_IMPLEMENTED("");
//        }
//        os << "\n";
//    }
//
//    void dump(IntConstant &ins) {
//        os << Dest(ins.getDest()) << "int constant " << ins.getValue();
//    }
//
//    void dump(GetLocal &ins) {
//        os << Dest(ins.getDest()) << "get " << Local(ins.getSlot());
//    }
//
//    void dump(SetLocal &ins) {
//        os << "set " << Local(ins.getSlot()) << " = " << Temp(ins.getSrc());
//    }
//
//    void dump(LoadString &ins) {
//        os << Dest(ins.getDest()) << "load string str." << ins.getId();
//    }
//
//    void dump(RefInc &ins) {
//        os << "ref inc " << Temp(ins.getTemp());
//    }
//
//    void dump(RefDec &ins) {
//        os << "ref dec " << Temp(ins.getTemp()) << LPad(ins.getLpad());
//    }
//
//    void dump(RefDecNoexcept &ins) {
//        os << "ref dec noexcept " << Temp(ins.getTemp()) << " [combine " << Temp(ins.getE()) << "]";
//    }
//
//    void dump(ReadLockGlobal &ins) {
//        os << "read lock " << Global(ins.getId());
//    }
//
//    void dump(ReadUnlockGlobal &ins) {
//        os << "read unlock " << Global(ins.getId());
//    }
//
//    void dump(WriteLockGlobal &ins) {
//        os << "write lock " << Global(ins.getId());
//    }
//
//    void dump(WriteUnlockGlobal &ins) {
//        os << "write unlock " << Global(ins.getId());
//    }
//
//    void dump(GetGlobal &ins) {
//        os << Dest(ins.getDest()) << "get " << Global(ins.getId());
//    }
//
//    void dump(SetGlobal &ins) {
//        os << "set " << Global(ins.getId()) << " = " << Temp(ins.getSrc());
//    }
//
//    void dump(MakeGlobal &ins) {
//        os << "make " << Global(ins.getId()) << " = " << Temp(ins.getSrc());
//    }
//
//    void dump(FreeGlobal &ins) {
//        os << Dest(ins.getDest()) << "free " << Global(ins.getId());
//    }
//
//    void dump(LandingPad &ins) {
//        os << Dest(ins.getDest()) << "landing pad";
//    }
//
//    void dump(Rethrow &ins) {
//        os << "rethrow " << Temp(ins.getE());
//    }
//
//    void dump(BinaryOperator &ins) {
//        os << Dest(ins.getDest()) << "binary operator " << &ins.getDesc() << " " << Temp(ins.getLeft()) << ", "
//                << Temp(ins.getRight()) << LPad(ins.getLpad());
//    }
//
//    void dump(Conversion &ins) {
//        os << Dest(ins.getDest()) << "conversion " << static_cast<int>(ins.getDesc().conversion) << " "
//                << Temp(ins.getArg()) << LPad(ins.getLpad());
//    }
//
//    void dump(RetVoid &ins) {
//        os << "ret void";
//    }
//
//private:
//    class Temp {
//    public:
//        explicit Temp(Id id) : id(id) {
//        }
//
//    private:
//        Id id;
//
//        friend std::ostream &operator<<(std::ostream &os, const Temp &t) {
//            return os << "temp." << t.id;
//        }
//    };
//
//    class Dest {
//    public:
//        explicit Dest(Id id) : id(id) {
//        }
//
//    private:
//        Id id;
//
//        friend std::ostream &operator<<(std::ostream &os, const Dest &d) {
//            return os << "temp." << d.id << " = ";
//        }
//    };
//
//    class Local {
//    public:
//        explicit Local(Id id) : id(id) {
//        }
//
//    private:
//        Id id;
//
//        friend std::ostream &operator<<(std::ostream &os, const Local &l) {
//            return os << "local." << l.id;
//        }
//    };
//
//    class Global {
//    public:
//        explicit Global(Id id) : id(id) {
//        }
//
//    private:
//        Id id;
//
//        friend std::ostream &operator<<(std::ostream &os, const Global &g) {
//            return os << "global." << g.id;
//        }
//    };
//
//    class LPad {
//    public:
//        explicit LPad(Id id) : id(id) {
//        }
//
//    private:
//        Id id;
//
//        friend std::ostream &operator<<(std::ostream &os, const LPad &l) {
//            os << " [";
//            if (l.id == InvalidId) {
//                os << "no lpad";
//            } else {
//                os << "lpad BB." << l.id;
//            }
//            return os << "]";
//        }
//    };
//
//private:
//    std::unordered_map<Instruction *, Id> insIndices;
//    std::ostream &os;
//};
//
//inline void Builder::dump() {
//    Dump d;
//    d.dump(f);
//}

} // namespace as
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AS_IS_H_
