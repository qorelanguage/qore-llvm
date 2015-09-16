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
#ifndef INCLUDE_QORE_IL_BUILDER_H_
#define INCLUDE_QORE_IL_BUILDER_H_

#include "qore/common/Logging.h"
#include "qore/common/Loggable.h"

#ifndef QORE_LOG_COMPONENT
#define QORE_LOG_COMPONENT "X"
#endif

namespace qore {
namespace il {

/**
 * \brief Represents the value of an expression.
 */
class AValue : public Loggable {

public:
    /**
     * \brief Reference type.
     */
    using Ref = std::shared_ptr<AValue>;

public:
    virtual ~AValue() = default;

    mutable void *tag;

protected:
    AValue() : tag(nullptr) {}

private:
    AValue(const AValue &) = delete;
    AValue(AValue &&) = delete;
    AValue &operator=(const AValue &) = delete;
    AValue &operator=(AValue &&) = delete;
};

class Constant : public AValue {

public:
    Constant(std::string value) : value(std::move(value)) {
        LOG("Create " << this);
    }
    ~Constant() {
        LOG("Destroy " << this);
    }

    const std::string &getValue() const {
        return value;
    }

protected:
    WRITE_TO_LOG("constant \"" << value << "\"");

private:
    Constant(const Constant &) = delete;
    Constant(Constant &&) = delete;
    Constant &operator=(const Constant &) = delete;
    Constant &operator=(Constant &&) = delete;

private:
    std::string value;
};

class LocalVariable : public AValue {

public:
    LocalVariable(std::string name) : name(std::move(name)) {
        LOG("Create " << this);
    }
    ~LocalVariable() {
        LOG("Destroy " << this);
    }

protected:
    WRITE_TO_LOG("local variable " << name);

private:
    LocalVariable(const LocalVariable &) = delete;
    LocalVariable(LocalVariable &&) = delete;
    LocalVariable &operator=(const LocalVariable &) = delete;
    LocalVariable &operator=(LocalVariable &&) = delete;

private:
    std::string name;
};

class TemporaryVariable : public AValue {

public:
    TemporaryVariable(int id) : id(id) {
        LOG("Create " << this);
    }
    ~TemporaryVariable() {
        LOG("Destroy " << this);
    }

protected:
    WRITE_TO_LOG("temporary #" << id);

private:
    TemporaryVariable(const TemporaryVariable &) = delete;
    TemporaryVariable(TemporaryVariable &&) = delete;
    TemporaryVariable &operator=(const TemporaryVariable &) = delete;
    TemporaryVariable &operator=(TemporaryVariable &&) = delete;

private:
    int id;
};

/**
 * \private
 */
class Action {
public:
    enum Type {
        Add,
        Assign,
        LifetimeStart,
        LifetimeEnd,
        Print,
        Return,
        Trim,
    };

    Action(Type type, const AValue *s1 = nullptr, const AValue *s2 = nullptr, const AValue *s3 = nullptr) : type(type), s1(s1), s2(s2), s3(s3) {
//        LOG("Creating action " << *this);
    }

    //COPY/MOVE
//private:
    Type type;
    const AValue *s1;
    const AValue *s2;
    const AValue *s3;

    friend std::ostream &operator<<(std::ostream &, const Action &);
};

inline std::ostream &operator<<(std::ostream &os, const Action &action) {
    switch (action.type) {
        case Action::Add:
            return os << "Add: " << action.s2 << " and " << action.s3 << " into " << action.s1;
        case Action::Assign:
            return os << "Assign: " << action.s2 << " into " << action.s1;
        case Action::LifetimeStart:
            return os << "LifetimeStart: " << action.s1;
        case Action::LifetimeEnd:
            return os << "LifetimeEnd: " << action.s1;
        case Action::Print:
            return os << "Print: " << action.s1;
        case Action::Return:
            return os << "Return";
        case Action::Trim:
            return os << "Trim: " << action.s1;
    }
//    QORE_UNREACHABLE("Invalid action");
    return os;
}

class Function {

public:
    ~Function() {
        //all constants should have use_count == 1
    }
    void dump() const {
        std::cout << "DUMP START\n";
        for (const Action &a : actions) {
            std::cout << a << "\n";
        }
        std::cout << "DUMP END\n";
    }

    //COPY/MOVE
//private:
    std::vector<std::unique_ptr<AValue>> objects;
    std::vector<std::shared_ptr<Constant>> constants;
    std::vector<Action> actions;
};

class Builder {

public:
    using Value = AValue::Ref;
    using LValue = AValue::Ref;

public:
    Builder() : f(new Function()), tempCount(0) {
    }

    std::unique_ptr<Function> build() {
        return std::move(f);
    }

    Value createStringConstant(const std::string &value, const SourceRange &range) {
        Constant *c = new Constant(value);
        f->constants.emplace_back(c);   //uses normal deleter
        return f->constants.back();
    }
    Value createLocalVariable(const std::string &name, const SourceRange &range) {
        return createObject(new LocalVariable(name));
    }
    Value createTemporaryVariable(const SourceRange &range) {
        return createObject(new TemporaryVariable(tempCount++));
    }
    void add(Value dest, Value left, Value right) {
        a(Action::Type::Add, dest, left, right);
    }
    void assign(Value left, Value right) {
        a(Action::Type::Assign, left, right);
    }
    void trim(Value lValue) {
        a(Action::Type::Trim, lValue);
    }
    void print(Value value) {
        a(Action::Type::Print, value);
    }
    void finalize() {
        a(Action::Type::Return);
    }

private:
    Value createObject(AValue *s) {
        f->objects.emplace_back(s);
        f->actions.push_back(Action(Action::LifetimeStart, s));
        return std::shared_ptr<AValue>(s, [this](AValue *s){
            f->actions.push_back(Action(Action::LifetimeEnd, s));
        });
    }

    void a(Action::Type type, const Value &v1 = nullptr, const Value &v2 = nullptr, const Value &v3 = nullptr) {
        f->actions.push_back(Action(type, v1.get(), v2.get(), v3.get()));
    }

private:
    std::unique_ptr<Function> f;
    int tempCount;
};

} // namespace il
} // namespace qore

#endif // INCLUDE_QORE_IL_BUILDER_H_
