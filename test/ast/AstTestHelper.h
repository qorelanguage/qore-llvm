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
#ifndef TEST_AST_ASTTESTHELPER_H_
#define TEST_AST_ASTTESTHELPER_H_

#include <memory>
#include "gmock/gmock.h"
#include "qore/ast/Expression.h"
#include "qore/ast/Statement.h"
#include "qore/ast/Visitor.h"

template<typename D, typename B>
typename D::Ptr ast_cast(std::unique_ptr<B> &&bPtr, const char *file, int line, const char *valueS, const char *typeS) {
    D *d = dynamic_cast<D *>(bPtr.get());
    if (d == nullptr) {
        ADD_FAILURE_AT(file, line) << valueS << " was expected to return "
                << typeS << ", but returned " << typeid(*bPtr.get()).name() << " instead";
    } else {
        bPtr.release();
    }
    return typename D::Ptr(d);
}

template<typename D, typename B>
D *ast_cast(const std::unique_ptr<B> &bPtr, const char *file, int line, const char *valueS, const char *typeS) {
    D *d = dynamic_cast<D *>(bPtr.get());
    if (d == nullptr) {
        ADD_FAILURE_AT(file, line) << valueS << " was expected to return "
                << typeS << ", but returned " << typeid(*bPtr.get()).name() << " instead";
    }
    return d;
}

#define AST_CAST(Type, Name, Value) \
    auto Name = ast_cast<Type>(Value, __FILE__, __LINE__, #Value, #Type); \
    if (!Name) return;

namespace qore {
namespace ast {

class MockExpression : public Expression {
public:
    using Ptr = std::unique_ptr<MockExpression>;

    MOCK_CONST_METHOD1(accept, void*(Visitor &));

    static Ptr create() {
        return Ptr(new MockExpression());
    }
};

class MockStatement : public Statement {
public:
    using Ptr = std::unique_ptr<MockStatement>;

    MOCK_CONST_METHOD1(accept, void*(Visitor &));

    static Ptr create() {
        return Ptr(new MockStatement());
    }
};

class MockVisitor : public Visitor {
public:
    MOCK_METHOD1(visit, void*(const class IntegerLiteral *));
    MOCK_METHOD1(visit, void*(const class StringLiteral *));
    MOCK_METHOD1(visit, void*(const class BinaryExpression *));
    MOCK_METHOD1(visit, void*(const class EmptyStatement *));
    MOCK_METHOD1(visit, void*(const class PrintStatement *));
    MOCK_METHOD1(visit, void*(const class Program *));
};

} // namespace ast
} // namespace qore

#endif // TEST_AST_ASTTESTHELPER_H_
