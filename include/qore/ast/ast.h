#ifndef INCLUDE_QORE_AST_AST_H_
#define INCLUDE_QORE_AST_AST_H_

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

/**
 * \private
 */
class Visitor {
public:
    using R = void*;

    virtual ~Visitor() {}
    virtual R visit(const class IntegerLiteral *) = 0;
    virtual R visit(const class StringLiteral *) = 0;
    virtual R visit(const class VariableLoadExpression *) = 0;
    virtual R visit(const class Assignment *) = 0;
    virtual R visit(const class BinaryExpression *) = 0;
    virtual R visit(const class EmptyStatement *) = 0;
    virtual R visit(const class ExpressionStatement *) = 0;
    virtual R visit(const class PrintStatement *) = 0;
    virtual R visit(const class Program *) = 0;
};

/**
 * \private
 */
class H {
public:
    H(char *s) : s(s) {
        free(s);
    }
    std::string s;
};

/**
 * \private
 */
class AstNode {

protected:
    AstNode() { }

public:
    virtual ~AstNode() { }
    AstNode(const AstNode &) = delete;
    AstNode(AstNode &&) = delete;
    AstNode &operator=(const AstNode &) = delete;
    AstNode &operator=(AstNode &&) = delete;

    virtual Visitor::R accept(Visitor &v) const = 0;

protected:
    virtual std::ostream &out(std::ostream &os) const = 0;
    friend std::ostream &operator<<(std::ostream &, const AstNode *);
};

inline std::ostream &operator<<(std::ostream &os, const AstNode *node) {
    return node->out(os);
}

/**
 * \private
 */
class Expression : public AstNode {

protected:
    Expression() { }

public:
    virtual ~Expression() { }
    Expression(const Expression &) = delete;
    Expression(Expression &&) = delete;
    Expression &operator=(const Expression &) = delete;
    Expression &operator=(Expression &&) = delete;
};

/**
 * \private
 */
class IntegerLiteral : public Expression {

public:
    IntegerLiteral(int value) : value(value) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    int getValue() const {
        return value;
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "IntegerLiteral(" << value << ")";
    }

private:
    int value;
};

/**
 * \private
 */
class StringLiteral : public Expression {

public:
    StringLiteral(H valueH) : value(valueH.s) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    const std::string &getValue() const {
        return value;
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "StringLiteral(\"" << value << "\")";
    }

private:
    std::string value;
};

/**
 * \private
 */
class VariableLoadExpression : public Expression {

public:
    VariableLoadExpression(H nameH) : name(std::move(nameH.s)) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    const std::string &getName() const {
        return name;
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "VariableLoadExpression(" << name << ")";
    }

private:
    std::string name;
};

/**
 * \private
 */
class Assignment : public Expression {

public:
    Assignment(H varNameH, Expression *value) : varName(std::move(varNameH.s)), value(value) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    const std::string &getVarName() const {
        return varName;
    }

    const Expression *getValue() const {
        return value.get();
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "Assignment(" << varName << " = " << value.get() << ")";
    }

private:
    std::string varName;
    std::unique_ptr<Expression> value;
};

/**
 * \private
 */
class BinaryExpression : public Expression {

public:
    BinaryExpression(Expression *left, Expression *right) : left(left), right(right) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    const Expression *getLeft() const {
        return left.get();
    }

    const Expression *getRight() const {
        return right.get();
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "BinaryExpression(" << left.get() << " + " << right.get() << ")";
    }

private:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
};

/**
 * \private
 */
class Statement : public AstNode {

protected:
    Statement() { }

public:
    virtual ~Statement() { }
    Statement(const Statement &) = delete;
    Statement(Statement &&) = delete;
    Statement &operator=(const Statement &) = delete;
    Statement &operator=(Statement &&) = delete;
};

/**
 * \private
 */
class EmptyStatement : public Statement {

public:
    EmptyStatement() {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "EmptyStatement";
    }
};

/**
 * \private
 */
class ExpressionStatement : public Statement {

public:
    ExpressionStatement(Expression *expression) : expression(expression) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    const Expression *getExpression() const {
        return expression.get();
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "ExpressionStatement(" << expression.get() << ")";
    }

private:
    std::unique_ptr<Expression> expression;
};

/**
 * \private
 */
class PrintStatement : public Statement {

public:
    PrintStatement(Expression *expression) : expression(expression) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    const Expression *getExpression() const {
        return expression.get();
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        return os << "PrintStatement(" << expression.get() << ")";
    }

private:
    std::unique_ptr<Expression> expression;
};

/**
 * \private
 */
class Program : public AstNode {

public:
    Program(std::vector<std::unique_ptr<Statement>> body) : body(std::move(body)) {
    }

    Visitor::R accept(Visitor &v) const override {
        return v.visit(this);
    }

    void forEachStatement(std::function<void(const Statement*)> f) const {
        for (const auto &s : body) {
            f(s.get());
        }
    }

//FIXME temp hack for interactive mode
    std::unique_ptr<Statement> getSingleStatement() {
        return body.empty() ? std::unique_ptr<Statement>(new EmptyStatement()) : std::move(body[0]);
    }

protected:
    std::ostream &out(std::ostream &os) const override {
        os << "Program:" << std::endl;
        for (const auto &s : body) {
            os << "  " << s.get() << std::endl;
        }
        return os;
    }

private:
    std::vector<std::unique_ptr<Statement>> body;
};

/**
 * \private
 */
class ProgramBuilder {

public:
    ProgramBuilder *addStatement(Statement *s) {
        body.push_back(std::unique_ptr<Statement>(s));
        return this;
    }

    Program *build() {
        return new Program(std::move(body));
    }

private:
    std::vector<std::unique_ptr<Statement>> body;
};

#endif /* INCLUDE_QORE_AST_AST_H_ */
