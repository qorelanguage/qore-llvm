#ifndef TOOLS_DRIVER_AST_H_
#define TOOLS_DRIVER_AST_H_

#include <memory>
#include <string>
#include <vector>

#include <iostream>
#include <cstring>

inline std::string i(int n) {
    return std::string(2*n, ' ');
}

class H {
public:
    H(char *s) : s(s) {
        free(s);
    }
    std::string s;
};

class Expression {
protected:
    Expression() { }
public:
    virtual ~Expression() { }
    Expression(const Expression &) = delete;
    Expression(Expression &&) = delete;
    Expression &operator=(const Expression &) = delete;
    Expression &operator=(Expression &&) = delete;

    virtual void dump(int indent = 0) const {};
    virtual std::ostream &out(std::ostream &os) const = 0;
};

inline std::ostream &operator<<(std::ostream &os, Expression *e) {
    return e->out(os);
}

class IntegerLiteral : public Expression {
public:
    IntegerLiteral(int value) : value(value) {
        std::cout << "CREATE IntegerLiteral(" << value << ")" << std::endl;
    }
    ~IntegerLiteral() {
        std::cout << "DESTROY IntegerLiteral(" << value << ")" << std::endl;
    }

    std::ostream &out(std::ostream &os) const override {
        return os << value;
    }

    void dump(int indent = 0) const override {
        std::cout << "IntegerLiteral " << value << std::endl;
    }

private:
    int value;
};

class StringLiteral : public Expression {
public:
    StringLiteral(H valueH) : value(valueH.s) {
        std::cout << "CREATE StringLiteral(" << value << ")" << std::endl;
    }
    ~StringLiteral() {
        std::cout << "DESTROY StringLiteral(" << value << ")" << std::endl;
    }

    std::ostream &out(std::ostream &os) const override {
        return os << '"' << value << '"';
    }

    void dump(int indent = 0) const override {
        std::cout << "StringLiteral \"" << value << '"' << std::endl;
    }

private:
    std::string value;
};

class VariableLoadExpression : public Expression {
public:
    VariableLoadExpression(H nameH) : name(std::move(nameH.s)) {
        std::cout << "CREATE VariableLoadExpression(" << name << ")" << std::endl;
    }
    ~VariableLoadExpression() {
        std::cout << "DESTROY VariableLoadExpression(" << name << ")" << std::endl;
    }

    std::ostream &out(std::ostream &os) const override {
        return os << name;
    }

    void dump(int indent = 0) const override {
        std::cout << "VariableLoadExpression " << name << std::endl;
    }

private:
    std::string name;
};

class Assignment : public Expression {
public:
    Assignment(H varNameH, Expression *value) : varName(std::move(varNameH.s)), value(value) {
        std::cout << "CREATE Assignment(" << varName << " = " << value << ")" << std::endl;
    }

    ~Assignment() {
        std::cout << "DESTROY Assignment(" << varName << " = " << value.get() << ")" << std::endl;
    }

    std::ostream &out(std::ostream &os) const override {
        return os << varName << " = " << value.get();
    }

    void dump(int indent = 0) const override {
        std::cout << "Assignment {" << std::endl;
        std::cout << i(indent + 1) << "varName: " << varName << std::endl;
        std::cout << i(indent + 1) << "value: ";
        value->dump(indent + 1);
        std::cout << i(indent) << "}" << std::endl;
    }

private:
    std::string varName;
    std::unique_ptr<Expression> value;
};

class Statement {
protected:
    Statement() { }
public:
    virtual ~Statement() { }
    Statement(const Statement &) = delete;
    Statement(Statement &&) = delete;
    Statement &operator=(const Statement &) = delete;
    Statement &operator=(Statement &&) = delete;
    virtual void dump(int indent = 0) const = 0;
};

class ExpressionStatement : public Statement {
public:
    ExpressionStatement(Expression *expression) : expression(expression) {
        std::cout << "CREATE ExpressionStatement(" << expression << ")" << std::endl;
    }

    ~ExpressionStatement() {
        std::cout << "DESTROY ExpressionStatement(" << expression.get() << ")" << std::endl;
    }

    void dump(int indent = 0) const override {
        std::cout << i(indent) << "ExpressionStatement: ";
        expression->dump(indent);
    }

private:
    std::unique_ptr<Expression> expression;
};

class Program {
public:
    Program() {
    }

    Program *add(Statement *s) {
        body.push_back(std::unique_ptr<Statement>(s));
        return this;
    }

    void dump(int indent = 0) const {
        std::cout << "Program {" << std::endl;
        for (const auto &s : body) {
            s->dump(indent + 1);
        }
        std::cout << i(indent) << "}" << std::endl;
    }

private:
    std::vector<std::unique_ptr<Statement>> body;
};

#endif /* TOOLS_DRIVER_AST_H_ */
