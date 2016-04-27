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
/// \brief Statement definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_STMT_STATEMENT_H_
#define INCLUDE_QORE_COMP_SEM_STMT_STATEMENT_H_

#include <memory>

namespace qore {
namespace comp {
namespace sem {

class CompoundStatement;
class ExpressionStatement;
class GlobalVariableInitializationStatement;
class IfStatement;
class ReturnStatement;
class TryStatement;

class Statement {

public:
    enum class Kind {
        Compound,
        Expression,
        GlobalVariableInitialization,
        If,
        Return,
        Try,
    };

public:
    using Ptr = std::unique_ptr<Statement>;

public:
    virtual ~Statement() = default;
    virtual Kind getKind() const = 0;

    template<typename V>
    typename V::ReturnType accept(V &v) const {
        #define CASE(K) case Kind::K:   return v.visit(static_cast<const K ## Statement &>(*this))
        switch (getKind()) {
            CASE(Compound);
            CASE(Expression);
            CASE(GlobalVariableInitialization);
            CASE(If);
            CASE(Return);
            CASE(Try);
            default:
                QORE_UNREACHABLE("");
        }
        #undef CASE
    }

protected:
    Statement() = default;

private:
    Statement(const Statement &) = delete;
    Statement(Statement &&) = delete;
    Statement &operator=(const Statement &) = delete;
    Statement &operator=(Statement &&) = delete;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_STMT_STATEMENT_H_
