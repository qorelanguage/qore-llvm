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
/// \brief Function definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IR_DECL_FUNCTION_H_
#define INCLUDE_QORE_IR_DECL_FUNCTION_H_

#include <string>
#include <vector>
#include "qore/common/Exceptions.h"
#include "qore/ir/decl/LocalVariable.h"
#include "qore/ir/stmt/Statement.h"
#include "qore/rt/Types.h"
#include "qore/rt/Meta.h"

namespace qore {
namespace ir {

class Function {

public:
    enum class Kind {
        User, Operator, Conversion, Box, AnyOperator
    };

public:
    using Ptr = std::unique_ptr<Function>;
    using Index = unsigned int;

public:
    virtual ~Function() = default;

    virtual Kind getKind() const = 0;
    virtual const Type &getRetType() const = 0;
    virtual const Index getArgCount() const = 0;
    virtual const Type &getArgType(Index index) const = 0;

    bool operator==(const Function &other) const {
        return this == &other;
    }

    bool operator!=(const Function &other) const {
        return !(*this == other);
    }

protected:
    Function() = default;

private:
    Function(const Function &) = delete;
    Function(Function &&) = delete;
    Function &operator=(const Function &) = delete;
    Function &operator=(Function &&) = delete;
};

class UserFunction : public Function {

public:
    using Ptr = std::unique_ptr<UserFunction>;

public:
    static Ptr create(std::string name) {
        return Ptr(new UserFunction(std::move(name)));
    }

    Kind getKind() const override {
        return Kind::User;
    }

    const Type &getRetType() const override {
        QORE_NOT_IMPLEMENTED("");
    }

    const Index getArgCount() const override {
        QORE_NOT_IMPLEMENTED("");
    }

    const Type &getArgType(Index index) const override {
        QORE_NOT_IMPLEMENTED("");
    }

    const LocalVariable &createLocalVariable(std::string name, const Type &type) { //FIXME StrRef, Type, Loc
        LocalVariable::Ptr ptr = LocalVariable::create(std::move(name), type);
        LocalVariable &lv = *ptr;
        locals.push_back(std::move(ptr));
        return lv;
    }

    void add(Statement::Ptr stmt) {
        statements.push_back(std::move(stmt));
    }

    const std::string &getName() const {
        return name;
    }

    const std::vector<LocalVariable::Ptr> &getLocalVariables() const {
        return locals;
    }

    const std::vector<Statement::Ptr> &getStatements() const {
        return statements;
    }

private:
    explicit UserFunction(std::string name) : name(std::move(name)) {
    }

private:
    std::string name;
    std::vector<LocalVariable::Ptr> locals;
    std::vector<Statement::Ptr> statements;
};

class OperatorFunction : public Function {

public:
    Kind getKind() const override {
        return Kind::Operator;
    }

    const Type &getRetType() const override {
        return retType;
    }

    const Index getArgCount() const override {
        return arg1Type == nullptr ? 1 : 2;
    }

    const Type &getArgType(Index index) const override {
        if (index == 0) {
            return arg0Type;
        }
        if (index == 1 && arg1Type) {
            return *arg1Type;
        }
        QORE_NOT_IMPLEMENTED("");
    }

    rt::Operator getOperator() const {
        return op;
    }

private:
    OperatorFunction(rt::Operator op, const Type &retType, const Type &arg0Type)
            : op(op), retType(retType), arg0Type(arg0Type), arg1Type(nullptr) {
    }

    OperatorFunction(rt::Operator op, const Type &retType, const Type &arg0Type, const Type &arg1Type)
            : op(op), retType(retType), arg0Type(arg0Type), arg1Type(&arg1Type) {
    }

private:
    rt::Operator op;
    const Type &retType;
    const Type &arg0Type;
    const Type *arg1Type;

    friend class Functions;
};

class ConversionFunction : public Function {

public:
    Kind getKind() const override {
        return Kind::Conversion;
    }

    const Type &getRetType() const override {
        return retType;
    }

    const Index getArgCount() const override {
        return 1;
    }

    const Type &getArgType(Index index) const override {
        if (index == 0) {
            return arg0Type;
        }
        QORE_NOT_IMPLEMENTED("");
    }

    rt::Conversion getConversion() const {
        return conv;
    }

private:
    ConversionFunction(rt::Conversion conv, const Type &retType, const Type &arg0Type)
            : conv(conv), retType(retType), arg0Type(arg0Type)  {
    }

private:
    rt::Conversion conv;
    const Type &retType;
    const Type &arg0Type;

    friend class Functions;
};

class BoxFunction : public Function {

public:
    Kind getKind() const override {
        return Kind::Box;
    }

    const Type &getRetType() const override {
        return Types::Any;
    }

    const Index getArgCount() const override {
        return 1;
    }

    const Type &getArgType(Index index) const override {
        if (index == 0) {
            return arg0Type;
        }
        QORE_NOT_IMPLEMENTED("");
    }

private:
    explicit BoxFunction(const Type &arg0Type) : arg0Type(arg0Type)  {
    }

private:
    const Type &arg0Type;

    friend class Functions;
};

class AnyOperatorFunction : public Function {

public:
    Kind getKind() const override {
        return Kind::AnyOperator;
    }

    const Type &getRetType() const override {
        return Types::Any;
    }

    const Index getArgCount() const override {
        return 2;
    }

    const Type &getArgType(Index index) const override {
        if (index == 0 || index == 1) {
            return Types::Any;
        }
        QORE_NOT_IMPLEMENTED("");
    }

private:
    AnyOperatorFunction() {
    }

private:
    friend class Functions;
};

class Functions {
public:
    static OperatorFunction StringPlusString;
    static OperatorFunction IntPlusInt;
    static ConversionFunction IntToString;
    static BoxFunction BoxInt;
    static AnyOperatorFunction AnyPlusAny;
};

} // namespace ir
} // namespace qore

#endif // INCLUDE_QORE_IR_DECL_FUNCTION_H_
