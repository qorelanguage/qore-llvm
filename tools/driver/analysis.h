#ifndef TOOLS_DRIVER_ANALYSIS_H_
#define TOOLS_DRIVER_ANALYSIS_H_

#include <cassert>
#include <map>
#include <string>
#include "qore/ast/BaseVisitor.h"

namespace qore {

/**
 * \private
 */
class Printable {

public:
    virtual ~Printable() {
    }

    virtual std::ostream &toStream(std::ostream &os) const = 0;
};

/**
 * \private
 */
std::ostream &operator<<(std::ostream &os, const Printable *p) {
    return p ? (p->toStream(os) << " <" << static_cast<const void*>(p) << ">") : os << "<nullptr>";
}

/**
 * \private
 */
std::ostream &operator<<(std::ostream &os, const std::shared_ptr<Printable> &p) {
    return os << p.get() << " [" << p.use_count() << "]";
}

/**
 * \private
 */
class Storage : public Printable {
};

/**
 * \private
 */
class Constant : public Storage {

public:
    using Ref = std::shared_ptr<Constant>;

    Constant(std::string value) : value(std::move(value)) {
        LOG("Create " << this);
    }

    ~Constant() {
        LOG("Destroy " << this);
    }

    std::ostream &toStream(std::ostream &os) const override {
        return os << "constant '" << value << "'";
    }

    const std::string &getValue() {
        return value;
    }

private:
    Constant(const Constant &) = delete;
    Constant(Constant &&) = delete;
    Constant &operator=(const Constant &) = delete;
    Constant &operator=(Constant &&) = delete;

private:
    std::string value;
};

/**
 * \private
 */
std::ostream &operator<<(std::ostream &os, const Constant::Ref &c) {
    return os << c.get() << " [" << c.use_count() << "]";
}

/**
 * \private
 */
class LocalVariable : public Storage {

public:
    using Ref = std::shared_ptr<LocalVariable>;

    LocalVariable(std::string name) : name(std::move(name)) {
        LOG("Create " << this);
    }

    ~LocalVariable() {
        LOG("Destroy " << this);
    }

    std::ostream &toStream(std::ostream &os) const override {
        return os << "local variable " << name;
    }

private:
    LocalVariable(const LocalVariable &) = delete;
    LocalVariable(LocalVariable &&) = delete;
    LocalVariable &operator=(const LocalVariable &) = delete;
    LocalVariable &operator=(LocalVariable &&) = delete;

private:
    std::string name;
};

/**
 * \private
 */
std::ostream &operator<<(std::ostream &os, const LocalVariable::Ref &v) {
    return os << v.get() << " [" << v.use_count() << "]";
}

/**
 * \private
 */
class TemporaryVariable : public Storage {

public:
    using Ref = std::shared_ptr<TemporaryVariable>;

    TemporaryVariable(int id) : id(id) {
        LOG("Create " << this);
    }

    ~TemporaryVariable() {
        LOG("Destroy " << this);
    }

    std::ostream &toStream(std::ostream &os) const override {
        return os << "temporary #" << id;
    }

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
std::ostream &operator<<(std::ostream &os, const TemporaryVariable::Ref &v) {
    return os << v.get() << " [" << v.use_count() << "]";
}

/**
 * \private
 */
class Value {

public:
    Value() {
    }

    ~Value() {
        if (ptr) {
            LOG("~Value of " << ptr);
        }
    }

    Value(const Value &) = default;
    Value(Value &&) = default;
    Value &operator=(const Value &) = default;
    Value &operator=(Value &&) = default;

private:
    Value(Constant::Ref t) : ptr(t) {
    }

    Value(TemporaryVariable::Ref t) : ptr(t) {
    }

    Value(LocalVariable::Ref t) : ptr(t) {
    }

    friend std::ostream &operator<<(std::ostream &os, const Value &value) {
        return os << "value of " << value.ptr;
    }

    friend class FunctionBuilder;

private:
    std::shared_ptr<Storage> ptr;
};

/**
 * \private
 */
class LValue {

public:
    LValue() {
    }

    ~LValue() {
        if (ptr) {
            LOG("~LValue of " << ptr);
        }
    }

    LValue(const LValue &) = default;
    LValue(LValue &&src) = default;
    LValue &operator=(const LValue &) = default;
    LValue &operator=(LValue &&) = default;

private:
    LValue(TemporaryVariable::Ref t) : ptr(t) {
    }

    LValue(LocalVariable::Ref t) : ptr(t) {
    }

    friend std::ostream &operator<<(std::ostream &os, const LValue &lvalue) {
        return os << "l-value " << lvalue.ptr;
    }

    friend class FunctionBuilder;

private:
    std::shared_ptr<Storage> ptr;
};

/**
 * \private
 */
class FunctionBuilder {

public:
    ~FunctionBuilder() {
        scope.clear();
        for (auto &var : temps) {
            assert(var.use_count() == 1);
        }
        for (auto &var : constants) {
            assert(var.use_count() == 1);
        }
        for (auto &var : variables) {
            assert(var.use_count() == 1);
        }
    }

    LValue createLocalVariable(const std::string &name) {
        return createLocalVariable0(name);
    }

    LValue resolveLocalVariable(const std::string &name) {
        return resolveLocalVariable0(name);
    }

    Value loadLocalVariable(const std::string &name) {
        LocalVariable::Ref v = resolveLocalVariable0(name);
        LOG("Load value of " << v)
        return v;
    }

    Value loadConstant(const std::string &value) {
        Constant::Ref c = createConstant0(value);
        LOG("Load value of " << c);
        return c;
    }

    void storeAndDiscard(LValue l, Value r) {
        LOG("Store " << r << " into " << l << " (discard)");
    }

//    Value store(LValue l, Value r) {
//        LOG("Store " << r << " into " << l << " (keep)");
//        return r;           //TODO implicit conversions : int z; short x; int y = 65536; z = x = y; => z is one?
//    }

    void trimAndDiscard(LValue l) {
        LOG("Trim " << l << " (discard)");
    }

    Value add(Value l, Value r) {
        TemporaryVariable::Ref t = temp();
        assert(t.use_count() == 2);
        LOG("Add " << l << " and " << r << " into " << t);
        return Value(t);
    }

    void print(Value v) {
        LOG("Print " << v);
    }

private:
    LocalVariable::Ref createLocalVariable0(const std::string &name) {
        //TODO exception safety, duplicates etc.
        LocalVariable *v = new LocalVariable(name);
        variables.emplace_back(v);
        scope[name] = variables.back();
        return variables.back();
    }

    LocalVariable::Ref resolveLocalVariable0(const std::string &name) {
        auto &x = scope[name];
        if (!x) {
            std::cout << "ERROR: undeclared\n";
            return createLocalVariable0(name);
        } else {
            return x;
        }
    }

    Constant::Ref createConstant0(const std::string &value) {
        for (const auto &c : constants) {
            if (c->getValue() == value) {
                return c;
            }
        }
        constants.emplace_back(new Constant(value));
        return constants.back();
    }

    //TODO: temps reuse stack space -> but this will be needed for locals as well, consider:
    //          if (...) { int a; ... } else { double b; ... }
    // In fact - what is the difference between a local variable an a temp? Temps just go out of scope immediately.
    TemporaryVariable::Ref temp() {
        for (const auto &t : temps) {
            if (t.unique()) {
                return t;
            }
        }
        temps.emplace_back(new TemporaryVariable(temps.size()));
        return temps.back();
    }

private:
    std::vector<LocalVariable::Ref> variables;
    std::vector<Constant::Ref> constants;
    std::vector<TemporaryVariable::Ref> temps;
    std::map<std::string, LocalVariable::Ref> scope;
};

/**
 * \private
 */
class LValueExpressionEvaluator : public ast::BaseVisitor {

public:
    LValueExpressionEvaluator(FunctionBuilder &builder) : BaseVisitor("LValueExpressionEvaluator"), builder(builder) {
    }

    void visit(const ast::VarDecl *node) override {
        LOG_FUNCTION();
        result = builder.createLocalVariable(node->name);
    }

    void visit(const ast::Identifier *node) override {
        LOG_FUNCTION();
        result = builder.resolveLocalVariable(node->name);
    }

    operator LValue() {
        return std::move(result);
    }

private:
    FunctionBuilder &builder;
    LValue result;
};

/**
 * \private
 */
class ValueExpressionEvaluator : public ast::BaseVisitor {

public:
    ValueExpressionEvaluator(FunctionBuilder &builder) : BaseVisitor("ValueExpressionEvaluator"), builder(builder) {
    }

    void visit(const ast::StringLiteral *node) override {
        LOG_FUNCTION();
        result = builder.loadConstant(node->value);
    }

    void visit(const ast::Identifier *node) override {
        LOG_FUNCTION();
        result = builder.loadLocalVariable(node->name);
    }

    void visit(const ast::BinaryExpression *node) override {
        LOG_FUNCTION();

        ValueExpressionEvaluator l(builder);
        node->left->accept(l);

        ValueExpressionEvaluator r(builder);
        node->right->accept(r);

        result = builder.add(l, r);
    }

    operator Value() {
        return std::move(result);
    }

private:
    FunctionBuilder &builder;
    Value result;
};

/**
 * \private
 */
class NoValueExpressionEvaluator : public ast::BaseVisitor {

public:
    NoValueExpressionEvaluator(FunctionBuilder &builder) : BaseVisitor("NoValueExpressionEvaluator"), builder(builder) {
    }

    void visit(const ast::VarDecl *node) override {
        LOG_FUNCTION();
        builder.createLocalVariable(node->name);
    }

    void visit(const ast::Assignment *node) override {
        LOG_FUNCTION();

        LValueExpressionEvaluator l(builder);
        node->left->accept(l);

        ValueExpressionEvaluator r(builder);
        node->right->accept(r);

        builder.storeAndDiscard(l, r);
    }

    void visit(const ast::UnaryExpression *node) override {
        LOG_FUNCTION();

        LValueExpressionEvaluator val(builder);
        node->operand->accept(val);

        builder.trimAndDiscard(val);
    }

private:
    FunctionBuilder &builder;
};

/**
 * \private
 */
class AnalysisVisitor : public ast::BaseVisitor {

public:
    AnalysisVisitor() : BaseVisitor("AnalysisVisitor") {
    }

    void visit(const ast::ExpressionStatement *node) override {
        LOG_FUNCTION();
        NoValueExpressionEvaluator e(builder);
        node->expression->accept(e);
    }

    void visit(const ast::PrintStatement *node) override {
        LOG_FUNCTION();
        ValueExpressionEvaluator e(builder);
        node->expression->accept(e);

        builder.print(e);
    }

    void visit(const ast::Program *node) override {
        LOG_FUNCTION();
        for (const auto &stmt : node->body) {
            stmt->accept(*this);
        }
    }

private:
    FunctionBuilder builder;
};

} // namespace qore

#endif // TOOLS_DRIVER_ANALYSIS_H_
