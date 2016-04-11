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
#ifndef INCLUDE_QORE_AS_AS_H_
#define INCLUDE_QORE_AS_AS_H_

#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "qore/as/Id.h"
#include "qore/rt/Types.h"
#include "qore/rt/Meta.h"
#include "qore/common/Exceptions.h"

namespace qore {
namespace as {

class Instruction {

public:
    using Ptr = std::unique_ptr<Instruction>;

public:
    enum Kind {
        IntConstant,
        GetLocal,
        SetLocal,
        LoadString,
        RefInc,
        RefDec,
        RefDecNoexcept,
        ReadLockGlobal,
        ReadUnlockGlobal,
        WriteLockGlobal,
        WriteUnlockGlobal,
        GetGlobal,
        SetGlobal,
        MakeGlobal,
        FreeGlobal,
        LandingPad,
        Rethrow,
        BinaryOperator,
        Conversion,
        RetVoid,
    };

public:
    virtual ~Instruction() = default;

    virtual Kind getKind() const = 0;

protected:
    Instruction() = default;

private:
    Instruction(const Instruction &) = delete;
    Instruction(Instruction &&) = delete;
    Instruction &operator=(const Instruction &) = delete;
    Instruction &operator=(Instruction &&) = delete;
};

class F;
class Block {

public:
    using Ptr = std::unique_ptr<Block>;

public:
    static Ptr create(F &f, Id id) {
        return Ptr(new Block(f, id));
    }

private:
    Block(F &f, Id id) : f(f), id(id) {
    }

public:
    F &f;
    Id id;
    std::vector<Instruction::Ptr> instructions;
};






class IntConstant : public Instruction {

public:
    IntConstant(Id dest, rt::qint value) : dest(dest), value(value) {
    }

    Kind getKind() const override {
        return Kind::IntConstant;
    }

    Id getDest() const {
        return dest;
    }

    rt::qint getValue() const {
        return value;
    }

private:
    Id dest;
    rt::qint value;
};

class GetLocal : public Instruction {

public:
    GetLocal(Id dest, Id slot) : dest(dest), slot(slot) {
    }

    Kind getKind() const override {
        return Kind::GetLocal;
    }

    Id getDest() const {
        return dest;
    }

    Id getSlot() const {
        return slot;
    }

private:
    Id dest;
    Id slot;
};

class SetLocal : public Instruction {

public:
    SetLocal(Id slot, Id src) : slot(slot), src(src) {
    }

    Kind getKind() const override {
        return Kind::SetLocal;
    }

    Id getSlot() const {
        return slot;
    }

    Id getSrc() const {
        return src;
    }

private:
    Id slot;
    Id src;
};

class LoadString : public Instruction {

public:
    LoadString(Id dest, Id id) : dest(dest), id(id) {
    }

    Kind getKind() const override {
        return Kind::LoadString;
    }

    Id getDest() const {
        return dest;
    }

    Id getId() const {
        return id;
    }

private:
    Id dest;
    Id id;
};

class RefInc : public Instruction {

public:
    explicit RefInc(Id temp) : temp(temp) {
    }

    Kind getKind() const override {
        return Kind::RefInc;
    }

    Id getTemp() const {
        return temp;
    }

private:
    Id temp;
};

class RefDec : public Instruction {

public:
    RefDec(Id temp, Id lpad) : temp(temp), lpad(lpad) {
    }

    Kind getKind() const override {
        return Kind::RefDec;
    }

    Id getTemp() const {
        return temp;
    }

    Id getLpad() const {
        return lpad;
    }

private:
    Id temp;
    Id lpad;
};

class RefDecNoexcept : public Instruction {

public:
    RefDecNoexcept(Id temp, Id e) : temp(temp), e(e) {
    }

    Kind getKind() const override {
        return Kind::RefDecNoexcept;
    }

    Id getTemp() const {
        return temp;
    }

    Id getE() const {
        return e;
    }

private:
    Id temp;
    Id e;
};


class ReadLockGlobal : public Instruction {

public:
    explicit ReadLockGlobal(Id id) : id(id) {
    }

    Kind getKind() const override {
        return Kind::ReadLockGlobal;
    }

    Id getId() const {
        return id;
    }

private:
    Id id;
};

class ReadUnlockGlobal : public Instruction {

public:
    explicit ReadUnlockGlobal(Id id) : id(id) {
    }

    Kind getKind() const override {
        return Kind::ReadUnlockGlobal;
    }

    Id getId() const {
        return id;
    }

private:
    Id id;
};

class WriteLockGlobal : public Instruction {

public:
    explicit WriteLockGlobal(Id id) : id(id) {
    }

    Kind getKind() const override {
        return Kind::WriteLockGlobal;
    }

    Id getId() const {
        return id;
    }

private:
    Id id;
};

class WriteUnlockGlobal : public Instruction {

public:
    explicit WriteUnlockGlobal(Id id) : id(id) {
    }

    Kind getKind() const override {
        return Kind::WriteUnlockGlobal;
    }

    Id getId() const {
        return id;
    }

private:
    Id id;
};

class GetGlobal : public Instruction {

public:
    GetGlobal(Id dest, Id id) : dest(dest), id(id) {
    }

    Kind getKind() const override {
        return Kind::GetGlobal;
    }

    Id getDest() const {
        return dest;
    }

    Id getId() const {
        return id;
    }

private:
    Id dest;
    Id id;
};

class SetGlobal: public Instruction {

public:
    SetGlobal(Id id, Id src) : id(id), src(src) {
    }

    Kind getKind() const override {
        return Kind::SetGlobal;
    }

    Id getId() const {
        return id;
    }

    Id getSrc() const {
        return src;
    }

private:
    Id id;
    Id src;
};

class MakeGlobal: public Instruction {

public:
    MakeGlobal(Id id, Id src) : id(id), src(src) {
    }

    Kind getKind() const override {
        return Kind::MakeGlobal;
    }

    Id getId() const {
        return id;
    }

    Id getSrc() const {
        return src;
    }

private:
    Id id;
    Id src;
};

class FreeGlobal: public Instruction {

public:
    FreeGlobal(Id dest, Id id) : dest(dest), id(id) {
    }

    Kind getKind() const override {
        return Kind::FreeGlobal;
    }

    Id getDest() const {
        return dest;
    }

    Id getId() const {
        return id;
    }

private:
    Id dest;
    Id id;
};

class LandingPad : public Instruction {

public:
    explicit LandingPad(Id dest) : dest(dest) {
    }

    Kind getKind() const override {
        return Kind::LandingPad;
    }

    Id getDest() const {
        return dest;
    }

private:
    Id dest;
};

class Rethrow : public Instruction {

public:
    explicit Rethrow(Id e) : e(e) {
    }

    Kind getKind() const override {
        return Kind::Rethrow;
    }

    Id getE() const {
        return e;
    }

private:
    Id e;
};

class BinaryOperator : public Instruction {

public:
    BinaryOperator(Id dest, const rt::meta::BinaryOperatorDesc &desc, Id left, Id right, Id lpad)
            : dest(dest), desc(desc), left(left), right(right), lpad(lpad) {
    }

    Kind getKind() const override {
        return Kind::BinaryOperator;
    }

    Id getDest() const {
        return dest;
    }

    const rt::meta::BinaryOperatorDesc &getDesc() const {
        return desc;
    }

    Id getLeft() const {
        return left;
    }

    Id getRight() const {
        return right;
    }

    Id getLpad() const {
        return lpad;
    }

private:
    Id dest;
    const rt::meta::BinaryOperatorDesc &desc;
    Id left;
    Id right;
    Id lpad;
};

class Conversion : public Instruction {

public:
    Conversion(Id dest, const rt::meta::ConversionDesc &desc, Id arg, Id lpad)
            : dest(dest), desc(desc), arg(arg), lpad(lpad) {
    }

    Kind getKind() const override {
        return Kind::Conversion;
    }

    Id getDest() const {
        return dest;
    }

    const rt::meta::ConversionDesc &getDesc() const {
        return desc;
    }

    Id getArg() const {
        return arg;
    }

    Id getLpad() const {
        return lpad;
    }

private:
    Id dest;
    const rt::meta::ConversionDesc &desc;
    Id arg;
    Id lpad;
};

class RetVoid : public Instruction {

public:
    RetVoid() {
    }

    Kind getKind() const override {
        return Kind::RetVoid;
    }
};

class F {

public:
    using Ptr = std::unique_ptr<F>;

public:
    static Ptr create(std::string name) {
        return Ptr(new F(std::move(name)));
    }

    Block *createBlock() {
        Block::Ptr ptr = Block::create(*this, blocks.size());
        Block *b = ptr.get();
        blocks.push_back(std::move(ptr));
        return b;
    }

    void empty() {
        blocks.clear();
    }

private:
    explicit F(std::string name) : name(std::move(name)), localCount(0), tempCount(0) {
    }

public:
    std::string name;
    std::vector<Block::Ptr> blocks;
    Id localCount;
    Id tempCount;
};

class S {

public:
    std::vector<std::string> strings;
    Id globalCount;
    std::vector<std::unique_ptr<F>> functions;

    F *qinit;
    F *qmain;
    F *qdone;
};

class Builder {

public:
    explicit Builder(F &f) : f(f), currentBlock(f.createBlock()) {
    }

    explicit Builder(Block *currentBlock) : f(currentBlock->f), currentBlock(currentBlock) {
    }

    void createIntConstant(Id dest, rt::qint value) {
        ins(new IntConstant(dest, value));
    }

    void createGetLocal(Id dest, Id slot) {
        ins(new GetLocal(dest, slot));
    }

    void createSetLocal(Id slot, Id src) {
        ins(new SetLocal(slot, src));
    }

    void createLoadString(Id dest, Id id) {
        ins(new LoadString(dest, id));
    }

    void createRefInc(Id temp) {
        ins(new RefInc(temp));
    }

    void createRefDec(Id temp, Block *lpad) {
        ins(new RefDec(temp, lpad ? lpad->id : InvalidId));
    }

    void createRefDecNoexcept(Id temp, Id e) {
        ins(new RefDecNoexcept(temp, e));
    }

    void createReadLockGlobal(Id id) {
        ins(new ReadLockGlobal(id));
    }

    void createReadUnlockGlobal(Id id) {
        ins(new ReadUnlockGlobal(id));
    }

    void createWriteLockGlobal(Id id) {
        ins(new WriteLockGlobal(id));
    }

    void createWriteUnlockGlobal(Id id) {
        ins(new WriteUnlockGlobal(id));
    }

    void createGetGlobal(Id dest, Id id) {
        ins(new GetGlobal(dest, id));
    }

    void createSetGlobal(Id id, Id src) {
        ins(new SetGlobal(id, src));
    }

    void createMakeGlobal(Id id, Id src) {
        ins(new MakeGlobal(id, src));
    }

    void createFreeGlobal(Id dest, Id id) {
        ins(new FreeGlobal(dest, id));
    }

    void createLandingPad(Id dest) {
        ins(new LandingPad(dest));
    }

    void createRethrow(Id e) {
        ins(new Rethrow(e));
    }

    void createBinaryOperator(Id dest, const rt::meta::BinaryOperatorDesc &desc, Id left, Id right, Block *lpad) {
        ins(new BinaryOperator(dest, desc, left, right, lpad ? lpad->id : InvalidId));
    }

    void createConversion(Id dest, const rt::meta::ConversionDesc &desc, Id arg, Block *lpad) {
        ins(new Conversion(dest, desc, arg, lpad ? lpad->id : InvalidId));
    }

    void createRetVoid() {
        ins(new RetVoid());
    }

    void dump();

private:
    void ins(Instruction *i) {
        currentBlock->instructions.emplace_back(i);
    }

private:
    F &f;
    Block *currentBlock;
};


class Dump {

public:
    Dump() : os(std::cout) {
    }

    void dump(F &f) {
        for (auto &b : f.blocks) {
            dump(*b);
        }
    }

    void dump(Block &b) {
        std::cout << "BB." << b.id << "\n";

        Id i = 0;
        for (auto &ins : b.instructions) {
            insIndices[ins.get()] = i++;
        }

        for (auto &ins : b.instructions) {
            dump(*ins);
        }
        insIndices.clear();
    }

    void dump(Instruction &ins) {
        os << std::setw(3) << std::right << insIndices[&ins] << ": ";
        switch (ins.getKind()) {
            case Instruction::Kind::IntConstant:
                dump(static_cast<IntConstant &>(ins));
                break;
            case Instruction::Kind::GetLocal:
                dump(static_cast<GetLocal &>(ins));
                break;
            case Instruction::Kind::SetLocal:
                dump(static_cast<SetLocal &>(ins));
                break;
            case Instruction::Kind::LoadString:
                dump(static_cast<LoadString &>(ins));
                break;
            case Instruction::Kind::RefInc:
                dump(static_cast<RefInc &>(ins));
                break;
            case Instruction::Kind::RefDec:
                dump(static_cast<RefDec &>(ins));
                break;
            case Instruction::Kind::RefDecNoexcept:
                dump(static_cast<RefDecNoexcept &>(ins));
                break;
            case Instruction::Kind::ReadLockGlobal:
                dump(static_cast<ReadLockGlobal &>(ins));
                break;
            case Instruction::Kind::ReadUnlockGlobal:
                dump(static_cast<ReadUnlockGlobal &>(ins));
                break;
            case Instruction::Kind::WriteLockGlobal:
                dump(static_cast<WriteLockGlobal &>(ins));
                break;
            case Instruction::Kind::WriteUnlockGlobal:
                dump(static_cast<WriteUnlockGlobal &>(ins));
                break;
            case Instruction::Kind::GetGlobal:
                dump(static_cast<GetGlobal &>(ins));
                break;
            case Instruction::Kind::SetGlobal:
                dump(static_cast<SetGlobal &>(ins));
                break;
            case Instruction::Kind::MakeGlobal:
                dump(static_cast<MakeGlobal &>(ins));
                break;
            case Instruction::Kind::FreeGlobal:
                dump(static_cast<FreeGlobal &>(ins));
                break;
            case Instruction::Kind::LandingPad:
                dump(static_cast<LandingPad &>(ins));
                break;
            case Instruction::Kind::Rethrow:
                dump(static_cast<Rethrow &>(ins));
                break;
            case Instruction::Kind::BinaryOperator:
                dump(static_cast<BinaryOperator &>(ins));
                break;
            case Instruction::Kind::Conversion:
                dump(static_cast<Conversion &>(ins));
                break;
            case Instruction::Kind::RetVoid:
                dump(static_cast<RetVoid &>(ins));
                break;
            default:
                QORE_NOT_IMPLEMENTED("");
        }
        os << "\n";
    }

    void dump(IntConstant &ins) {
        os << Dest(ins.getDest()) << "int constant " << ins.getValue();
    }

    void dump(GetLocal &ins) {
        os << Dest(ins.getDest()) << "get " << Local(ins.getSlot());
    }

    void dump(SetLocal &ins) {
        os << "set " << Local(ins.getSlot()) << " = " << Temp(ins.getSrc());
    }

    void dump(LoadString &ins) {
        os << Dest(ins.getDest()) << "load string str." << ins.getId();
    }

    void dump(RefInc &ins) {
        os << "ref inc " << Temp(ins.getTemp());
    }

    void dump(RefDec &ins) {
        os << "ref dec " << Temp(ins.getTemp()) << LPad(ins.getLpad());
    }

    void dump(RefDecNoexcept &ins) {
        os << "ref dec noexcept " << Temp(ins.getTemp()) << " [combine " << Temp(ins.getE()) << "]";
    }

    void dump(ReadLockGlobal &ins) {
        os << "read lock " << Global(ins.getId());
    }

    void dump(ReadUnlockGlobal &ins) {
        os << "read unlock " << Global(ins.getId());
    }

    void dump(WriteLockGlobal &ins) {
        os << "write lock " << Global(ins.getId());
    }

    void dump(WriteUnlockGlobal &ins) {
        os << "write unlock " << Global(ins.getId());
    }

    void dump(GetGlobal &ins) {
        os << Dest(ins.getDest()) << "get " << Global(ins.getId());
    }

    void dump(SetGlobal &ins) {
        os << "set " << Global(ins.getId()) << " = " << Temp(ins.getSrc());
    }

    void dump(MakeGlobal &ins) {
        os << "make " << Global(ins.getId()) << " = " << Temp(ins.getSrc());
    }

    void dump(FreeGlobal &ins) {
        os << Dest(ins.getDest()) << "free " << Global(ins.getId());
    }

    void dump(LandingPad &ins) {
        os << Dest(ins.getDest()) << "landing pad";
    }

    void dump(Rethrow &ins) {
        os << "rethrow " << Temp(ins.getE());
    }

    void dump(BinaryOperator &ins) {
        os << Dest(ins.getDest()) << "binary operator " << &ins.getDesc() << " " << Temp(ins.getLeft()) << ", "
                << Temp(ins.getRight()) << LPad(ins.getLpad());
    }

    void dump(Conversion &ins) {
        os << Dest(ins.getDest()) << "conversion " << static_cast<int>(ins.getDesc().conversion) << " "
                << Temp(ins.getArg()) << LPad(ins.getLpad());
    }

    void dump(RetVoid &ins) {
        os << "ret void";
    }

private:
    class Temp {
    public:
        explicit Temp(Id id) : id(id) {
        }

    private:
        Id id;

        friend std::ostream &operator<<(std::ostream &os, const Temp &t) {
            return os << "temp." << t.id;
        }
    };

    class Dest {
    public:
        explicit Dest(Id id) : id(id) {
        }

    private:
        Id id;

        friend std::ostream &operator<<(std::ostream &os, const Dest &d) {
            return os << "temp." << d.id << " = ";
        }
    };

    class Local {
    public:
        explicit Local(Id id) : id(id) {
        }

    private:
        Id id;

        friend std::ostream &operator<<(std::ostream &os, const Local &l) {
            return os << "local." << l.id;
        }
    };

    class Global {
    public:
        explicit Global(Id id) : id(id) {
        }

    private:
        Id id;

        friend std::ostream &operator<<(std::ostream &os, const Global &g) {
            return os << "global." << g.id;
        }
    };

    class LPad {
    public:
        explicit LPad(Id id) : id(id) {
        }

    private:
        Id id;

        friend std::ostream &operator<<(std::ostream &os, const LPad &l) {
            os << " [";
            if (l.id == InvalidId) {
                os << "no lpad";
            } else {
                os << "lpad BB." << l.id;
            }
            return os << "]";
        }
    };

private:
    std::unordered_map<Instruction *, Id> insIndices;
    std::ostream &os;
};

inline void Builder::dump() {
    Dump d;
    d.dump(f);
}

} // namespace as
} // namespace qore

#endif // INCLUDE_QORE_AS_AS_H_
