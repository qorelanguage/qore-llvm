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
/// \brief AST nodes related to statements.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_STATEMENT_H_
#define INCLUDE_QORE_COMP_AST_STATEMENT_H_

#include "qore/comp/ast/Node.h"

#include <vector>

namespace qore {
namespace comp {
namespace ast {

class EmptyStatement;
class ExpressionStatement;
class CompoundStatement;
class ReturnStatement;
class IfStatement;
class TryStatement;
class ForeachStatement;
class ThrowStatement;
class SimpleStatement;
class ScopeGuardStatement;
class WhileStatement;
class DoWhileStatement;
class ForStatement;
class SwitchStatement;

/**
 * \brief Base class for all nodes representing statements.
 */
class Statement : public Node {

public:
    /**
     * \brief Identifies the concrete type of an instance.
     */
    enum class Kind {
        Empty,          //!< Identifies an instance of \ref EmptyStatement.
        Expression,     //!< Identifies an instance of \ref ExpressionStatement.
        Compound,       //!< Identifies an instance of \ref CompoundStatement.
        Return,         //!< Identifies an instance of \ref ReturnStatement.
        If,             //!< Identifies an instance of \ref IfStatement.
        Try,            //!< Identifies an instance of \ref TryStatement.
        Foreach,        //!< Identifies an instance of \ref ForeachStatement.
        Throw,          //!< Identifies an instance of \ref ThrowStatement.
        Simple,         //!< Identifies an instance of \ref SimpleStatement.
        ScopeGuard,     //!< Identifies an instance of \ref ScopeGuardStatement.
        While,          //!< Identifies an instance of \ref WhileStatement.
        DoWhile,        //!< Identifies an instance of \ref DoWhileStatement.
        For,            //!< Identifies an instance of \ref ForStatement.
        Switch,         //!< Identifies an instance of \ref SwitchStatement.
    };

public:
    using Ptr = std::unique_ptr<Statement>;                 //!< Pointer type.

    /**
     * \brief Returns the kind of the statement.
     * \return the kind of the statement
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
};

/**
 * \brief Represents an empty statement.
 */
class EmptyStatement : public Statement {

public:
    SourceLocation location;                                //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<EmptyStatement>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \param location the location of the semicolon token
     * \return a unique pointer to the allocated node
     */
    static Ptr create(SourceLocation location) {
        return Ptr(new EmptyStatement(location));
    }

    Kind getKind() const override {
        return Kind::Empty;
    }

    SourceLocation getStart() const override {
        return location;
    }

    SourceLocation getEnd() const override {
        return location;
    }

private:
    explicit EmptyStatement(SourceLocation location) : location(location) {
    }
};

/**
 * \brief Represents a block.
 */
class CompoundStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the opening curly brace.
    SourceLocation end;                                     //!< The location of the closing curly brace.
    std::vector<Statement::Ptr> statements;                 //!< The statements in the block.

public:
    using Ptr = std::unique_ptr<CompoundStatement>;         //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new CompoundStatement());
    }

    Kind getKind() const override {
        return Kind::Compound;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    CompoundStatement() {
    }
};

} // namespace ast
} // namespace comp
} // namespace qore

#include "qore/comp/ast/Expression.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Represents an expression statement.
 */
class ExpressionStatement : public Statement {

    //XXX guarantee nonull expression
public:
    SourceLocation start;                                   //!< The starting location.
    Expression::Ptr expression;                             //!< The expression.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<ExpressionStatement>;       //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ExpressionStatement());
    }

    Kind getKind() const override {
        return Kind::Expression;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    ExpressionStatement() {
    }
};

/**
 * \brief Represents a return statement.
 */
class ReturnStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'return' keyword.
    Expression::Ptr expression;                             //!< The optional expression.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<ReturnStatement>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ReturnStatement());
    }

    Kind getKind() const override {
        return Kind::Return;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    ReturnStatement() {
    }
};

/**
 * \brief Represents an if statement.
 */
class IfStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'if' keyword.
    Expression::Ptr condition;                              //!< The condition.
    Statement::Ptr stmtTrue;                                //!< The statement to execute if condition is true.
    Statement::Ptr stmtFalse;                               //!< The optional 'else' statement.

public:
    using Ptr = std::unique_ptr<IfStatement>;               //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new IfStatement());
    }

    Kind getKind() const override {
        return Kind::If;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmtFalse ? stmtFalse->getEnd() : stmtTrue->getEnd();
    }

private:
    IfStatement() {
    }
};

/**
 * \brief Represents a try statement.
 */
class TryStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'try' keyword.
    Statement::Ptr stmtTry;                                 //!< The body of the try statement.
    Type exceptionType;                                     //!< The type of the exception parameter.
    Token exceptionName;                                    //!< The name of the exception parameter.
    Statement::Ptr stmtCatch;                               //!< The body of the catch part.

public:
    using Ptr = std::unique_ptr<TryStatement>;              //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new TryStatement());
    }

    Kind getKind() const override {
        return Kind::Try;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmtCatch->getEnd();
    }

private:
    TryStatement() : exceptionType(Type::createInvalid()) {
    }
};

/**
 * \brief Represents a foreach statement.
 */
class ForeachStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'foreach' keyword.
    Type varType;                                           //!< The type of the variable.
    Token varName;                                          //!< The name of the variable.
    Expression::Ptr expr;                                   //!< The expression to iterate.
    Statement::Ptr stmt;                                    //!< The body of the loop.

public:
    using Ptr = std::unique_ptr<ForeachStatement>;          //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ForeachStatement());
    }

    Kind getKind() const override {
        return Kind::Foreach;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmt->getEnd();
    }

private:
    ForeachStatement() : varType(Type::createInvalid()) {
    }
};

/**
 * \brief Represents a throw statement.
 */
class ThrowStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'throw' keyword.
    std::vector<Expression::Ptr> exprList;                  //!< The list of expressions.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<ThrowStatement>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ThrowStatement());
    }

    Kind getKind() const override {
        return Kind::Throw;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    ThrowStatement() {
    }
};

/**
 * \brief Represents a statement consisting of a single keyword, e.g. `break;` or `rethrow;`.
 */
class SimpleStatement : public Statement {

public:
    Token keyword;                                          //!< The keyword.
    SourceLocation end;                                     //!< The location of the semicolon.

public:
    using Ptr = std::unique_ptr<SimpleStatement>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new SimpleStatement());
    }

    Kind getKind() const override {
        return Kind::Simple;
    }

    SourceLocation getStart() const override {
        return keyword.location;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    SimpleStatement() {
    }
};

/**
 * \brief Represents a scope guard statement such as on_error or on_exit.
 */
class ScopeGuardStatement : public Statement {

public:
    Token keyword;                                          //!< The keyword.
    Statement::Ptr stmt;                                    //!< The statement.

public:
    using Ptr = std::unique_ptr<ScopeGuardStatement>;       //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ScopeGuardStatement());
    }

    Kind getKind() const override {
        return Kind::ScopeGuard;
    }

    SourceLocation getStart() const override {
        return keyword.location;
    }

    SourceLocation getEnd() const override {
        return stmt->getEnd();
    }

private:
    ScopeGuardStatement() {
    }
};

/**
 * \brief Represents a while statement.
 */
class WhileStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'while' keyword.
    Expression::Ptr expr;                                   //!< The condition.
    Statement::Ptr stmt;                                    //!< The body of the loop.

public:
    using Ptr = std::unique_ptr<WhileStatement>;            //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new WhileStatement());
    }

    Kind getKind() const override {
        return Kind::While;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmt->getEnd();
    }

private:
    WhileStatement() {
    }
};

/**
 * \brief Represents a do-while statement.
 */
class DoWhileStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'do' keyword.
    Statement::Ptr stmt;                                    //!< The body of the loop.
    Expression::Ptr expr;                                   //!< The condition.
    SourceLocation end;                                     //!< The location of the semicolon keyword.

public:
    using Ptr = std::unique_ptr<DoWhileStatement>;          //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new DoWhileStatement());
    }

    Kind getKind() const override {
        return Kind::DoWhile;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    DoWhileStatement() {
    }
};

/**
 * \brief Represents a for statement.
 */
class ForStatement : public Statement {

public:
    SourceLocation start;                                   //!< The location of the 'for' keyword.
    Expression::Ptr init;                                   //!< The initialization expression.
    Expression::Ptr condition;                              //!< The condition.
    Expression::Ptr update;                                 //!< The update expression.
    Statement::Ptr stmt;                                    //!< The body of the loop.

public:
    using Ptr = std::unique_ptr<ForStatement>;              //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new ForStatement());
    }

    Kind getKind() const override {
        return Kind::For;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return stmt->getEnd();
    }

private:
    ForStatement() {
    }
};

/**
 * \brief Represents a switch statement.
 */
class SwitchStatement : public Statement {

public:
    /**
     * \brief A case block.
     */
    class CaseBlock {
    public:
        Token keyword;                                      //!< The 'case' or  'default' keyword.
        Token op;                                           //!< The optional operator.
        Expression::Ptr expr;                               //!< The optional expression.
        std::vector<Statement::Ptr> statements;             //!< The statements.
    public:
        using Ptr = std::unique_ptr<CaseBlock>;             //!< Pointer type.
    public:
        /**
         * \brief Allocates a new instance.
         * \return a unique pointer to the allocated instance
         */
        static Ptr create() {
            return Ptr(new CaseBlock());
        }
    private:
        CaseBlock() {
        }
    };

public:
    SourceLocation start;                                   //!< The location of the 'switch' keyword.
    Expression::Ptr expr;                                   //!< The expression.
    std::vector<CaseBlock::Ptr> body;                       //!< The body of the switch statement.
    SourceLocation end;                                     //!< The location of the closing brace.

public:
    using Ptr = std::unique_ptr<SwitchStatement>;           //!< Pointer type.

public:
    /**
     * \brief Allocates a new node.
     * \return a unique pointer to the allocated node
     */
    static Ptr create() {
        return Ptr(new SwitchStatement());
    }

    Kind getKind() const override {
        return Kind::Switch;
    }

    SourceLocation getStart() const override {
        return start;
    }

    SourceLocation getEnd() const override {
        return end;
    }

private:
    SwitchStatement() {
    }
};

template<typename V>
typename V::ReturnType Statement::accept(V &visitor) const {
    /// \cond NoDoxygen
    #define CASE(K) case Kind::K: return visitor.visit(static_cast<const K ## Statement &>(*this))
    /// \endcond NoDoxygen
    switch (getKind()) {
        CASE(Empty);
        CASE(Expression);
        CASE(Compound);
        CASE(Return);
        CASE(If);
        CASE(Try);
        CASE(Foreach);
        CASE(Throw);
        CASE(Simple);
        CASE(ScopeGuard);
        CASE(While);
        CASE(DoWhile);
        CASE(For);
        CASE(Switch);
        default:
            QORE_UNREACHABLE("");
    }
    #undef CASE
}

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_STATEMENT_H_
