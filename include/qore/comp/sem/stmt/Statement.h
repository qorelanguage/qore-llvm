//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 - 2020 Qore Technologies, s.r.o.
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

/**
 * \brief Base class for temporary representations of statements.
 */
class Statement {

public:
    /**
     * \brief Identifies the concrete type of an instance.
     */
    enum class Kind {
        Compound,                       //!< Identifies an instance of \ref CompoundStatement.
        Expression,                     //!< Identifies an instance of \ref ExpressionStatement.
        GlobalVariableInitialization,   //!< Identifies an instance of \ref GlobalVariableInitializationStatement.
        If,                             //!< Identifies an instance of \ref IfStatement.
        Return,                         //!< Identifies an instance of \ref ReturnStatement.
        Try,                            //!< Identifies an instance of \ref TryStatement.
    };

public:
    using Ptr = std::unique_ptr<Statement>;     //!< Pointer type.

public:
    virtual ~Statement() = default;

    /**
     * \brief Returns the kind of the expression.
     * \return the kind of the expression
     */
    virtual Kind getKind() const = 0;

    /**
     * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Statement.
     * \param visitor the visitor to call
     * \return the value returned from the visitor
     * \tparam V the type of the visitor
     */
    template<typename V>
    typename V::ReturnType accept(V &visitor) const;

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

#include "qore/comp/sem/stmt/CompoundStatement.h"
#include "qore/comp/sem/stmt/ExpressionStatement.h"
#include "qore/comp/sem/stmt/ReturnStatement.h"
#include "qore/comp/sem/stmt/GlobalVariableInitializationStatement.h"
#include "qore/comp/sem/stmt/IfStatement.h"
#include "qore/comp/sem/stmt/TryStatement.h"

namespace qore {
namespace comp {
namespace sem {

template<typename V>
typename V::ReturnType Statement::accept(V &visitor) const {
    /// \cond NoDoxygen
    #define CASE(K) case Kind::K:   return visitor.visit(static_cast<const K ## Statement &>(*this))
    /// \endcond NoDoxygen
    switch (getKind()) {
        CASE(Compound);
        CASE(Expression);
        CASE(GlobalVariableInitialization);
        CASE(If);
        CASE(Return);
        CASE(Try);
        default:
            QORE_UNREACHABLE("Invalid Statement::Kind: " << static_cast<int>(getKind()));
    }
    #undef CASE
}

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_STMT_STATEMENT_H_
