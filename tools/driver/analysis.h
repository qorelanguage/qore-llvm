#ifndef TOOLS_DRIVER_ANALYSIS_H_
#define TOOLS_DRIVER_ANALYSIS_H_

#include <cassert>
#include <map>
#include <string>
#include "qore/ast/Visitor.h"

namespace qore {

class FunctionBuilder;

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
 * \todo better name
 */
class Storage : public Printable {

public:
    using Ref = std::shared_ptr<Storage>;
};

/**
 * \private
 */
std::ostream &operator<<(std::ostream &os, const Storage::Ref &s) {
    return os << s.get() << " [" << s.use_count() << "]";
}

/**
 * \private
 */
class Constant : public Storage {

public:
    Constant(std::string value);
    ~Constant();

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
class LocalVariable : public Storage {

public:
    LocalVariable(std::string name);
    ~LocalVariable();

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
class TemporaryVariable : public Storage {

public:
    TemporaryVariable(int id);
    ~TemporaryVariable();

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
class Value {

public:
    Value() {
    }

    Value(const Value &) = default;
    Value(Value &&) = default;
    Value &operator=(const Value &) = default;
    Value &operator=(Value &&) = default;

private:
    Value(Storage::Ref t) : ptr(t) {
    }

    friend std::ostream &operator<<(std::ostream &os, const Value &value) {
        return os << "value of " << value.ptr;
    }

    friend class FunctionBuilder;

private:
    Storage::Ref ptr;
};

/**
 * \private
 */
class LValue {

public:
    LValue() {
    }

    LValue(const LValue &) = default;
    LValue(LValue &&src) = default;
    LValue &operator=(const LValue &) = default;
    LValue &operator=(LValue &&) = default;

    operator bool() const {
        return ptr != nullptr;
    }

    bool operator!() const {
        return ptr == nullptr;
    }

private:
    LValue(Storage::Ref t) : ptr(t) {
    }

    friend std::ostream &operator<<(std::ostream &os, const LValue &lvalue) {
        return os << "l-value " << lvalue.ptr;
    }

    friend class FunctionBuilder;

private:
    Storage::Ref ptr;
};

/**
 * \private
 */
class FunctionBuilder {

public:
    void finalize() {
        scope.clear();
        LOG("End of function");
    }

    LValue createLocalVariable(const std::string &name) {
        //TODO exception safety, duplicates etc.
        LValue var(createObject(new LocalVariable(name)));
        scope[name] = var;
        return var;
    }

    LValue resolveLocalVariable(const std::string &name) {
        auto &x = scope[name];
        if (!x) {
            LOG("ERROR: undeclared");
            return createLocalVariable(name);
        } else {
            return x;
        }
    }

    Value loadConstant(const std::string &value) {
        Value c(createObject(new Constant(value)));
        LOG("Load " << c);
        return c;
    }

    Value load(LValue &&lvalue) {
        LOG("Load value of " << lvalue.ptr);
        return Value(lvalue.ptr);
    }

    Value assign(LValue &&l, Value &&r) {
        LOG("Assign " << r << " into " << l);
        return load(std::move(l));
    }

    Value trim(LValue &&l) {
        LOG("Trim " << l);
        return load(std::move(l));
    }

    Value add(LValue &&dest, Value &&l, Value &&r) {
        if (!dest) {
            dest = createTemporary();
        }
        LOG("Add " << l << " and " << r << " into " << dest);
        return load(std::move(dest));
    }

    void print(Value &&v) {
        LOG("Print " << v);
    }

    void lifetimeStart(const Storage *s) {
        LOG("Lifetime start: " << s);
    }

    void lifetimeEnd(const Storage *s) {
        LOG("Lifetime end: " << s);
    }

private:
    Storage::Ref createObject(Storage *s) {
        objects.emplace_back(s);
        lifetimeStart(s);
        return std::shared_ptr<Storage>(s, [this](Storage *s){lifetimeEnd(s);});
    }

    LValue createTemporary() {
        return LValue(createObject(new TemporaryVariable(tempCount++)));
    }

private:
    std::vector<std::unique_ptr<Storage>> objects;
    std::map<std::string, LValue> scope;
    int tempCount{0};
};

/**
 * \private
 */
LValue evalLValue(FunctionBuilder &builder, const ast::Expression::Ptr &node);

/**
 * \private
 */
Value evalValue(FunctionBuilder &builder, const ast::Expression::Ptr &node, LValue &&dest = LValue());

/**
 * \private
 */
void eval(FunctionBuilder &builder, const ast::Expression::Ptr &node, LValue &&dest = LValue());

/**
 * \private
 */
class LValueExpressionEvaluator : public ast::ExpressionVisitor {

private:
    LValueExpressionEvaluator(FunctionBuilder &builder) : builder(builder) {
    }

public:
    void visit(const ast::IntegerLiteral *node) override {
        //TODO
    }

    void visit(const ast::StringLiteral *node) override {
        //TODO
    }

    void visit(const ast::BinaryExpression *node) override {
        //TODO
    }

    void visit(const ast::UnaryExpression *node) override {
        //TODO
    }

    void visit(const ast::Assignment *node) override {
        //TODO
    }

    void visit(const ast::VarDecl *node) override {
        result = builder.createLocalVariable(node->name);
    }

    void visit(const ast::Identifier *node) override {
        result = builder.resolveLocalVariable(node->name);
    }

private:
    FunctionBuilder &builder;
    LValue result;

    friend LValue evalLValue(FunctionBuilder &builder, const ast::Expression::Ptr &node);
};

/**
 * \private
 */
class ValueExpressionEvaluator : public ast::ExpressionVisitor {

protected:
    ValueExpressionEvaluator(FunctionBuilder &builder, bool needsValue, LValue dest) : builder(builder), needsValue(needsValue), dest(std::move(dest)) {
    }

public:
    void visit(const ast::IntegerLiteral *node) override {
        //TODO
    }

    void visit(const ast::StringLiteral *node) override {
        assign(builder.loadConstant(node->value));
        checkNoEffect();
    }

    void visit(const ast::BinaryExpression *node) override {
        Value l = evalValue(builder, node->left);
        result = builder.add(std::move(dest), std::move(l), evalValue(builder, node->right));
        checkNoEffect();
    }

    void visit(const ast::UnaryExpression *node) override {
        assign(builder.trim(evalLValue(builder, node->operand)));
    }

    void visit(const ast::Assignment *node) override {
        assign(evalValue(builder, node->right, evalLValue(builder, node->left)));
    }

    void visit(const ast::VarDecl *node) override {
        assign(builder.load(builder.createLocalVariable(node->name)));
    }

    void visit(const ast::Identifier *node) override {
        assign(builder.load(builder.resolveLocalVariable(node->name)));
        checkNoEffect();
    }

private:
    void assign(Value &&value) {
        result = !dest ? std::move(value) : builder.assign(std::move(dest), std::move(value));
    }

    void checkNoEffect() {
        if (!dest && !needsValue) {
            //error statement has no effect
        }
    }

private:
    FunctionBuilder &builder;
    bool needsValue;
    LValue dest;
    Value result;

    friend Value evalValue(FunctionBuilder &, const ast::Expression::Ptr &, LValue &&);
    friend void eval(FunctionBuilder &, const ast::Expression::Ptr &, LValue &&);
};

inline LValue evalLValue(FunctionBuilder &builder, const ast::Expression::Ptr &node) {
    LValueExpressionEvaluator r(builder);
    node->accept(r);
    return std::move(r.result);
}

inline Value evalValue(FunctionBuilder &builder, const ast::Expression::Ptr &node, LValue &&dest) {
    ValueExpressionEvaluator r(builder, true, std::move(dest));
    node->accept(r);
    return std::move(r.result);
}

inline void eval(FunctionBuilder &builder, const ast::Expression::Ptr &node, LValue &&dest) {
    ValueExpressionEvaluator r(builder, false, std::move(dest));
    node->accept(r);
}

/**
 * \private
 */
class AnalysisVisitor : private ast::StatementVisitor, public ast::ProgramVisitor {

public:
    AnalysisVisitor() {
    }

    void visit(const ast::EmptyStatement *node) override {
        LOG_FUNCTION();
    }

    void visit(const ast::ExpressionStatement *node) override {
        LOG_FUNCTION();
        eval(builder, node->expression);
    }

    void visit(const ast::PrintStatement *node) override {
        LOG_FUNCTION();
        builder.print(evalValue(builder, node->expression));
    }

    void visit(const ast::Program *node) override {
        LOG_FUNCTION();
        for (const auto &stmt : node->body) {
            stmt->accept(*this);
        }
        builder.finalize();
    }

private:
    FunctionBuilder builder;
};

Constant::Constant(std::string value) : value(std::move(value)) {
    LOG("Create " << this);
}

Constant::~Constant() {
    LOG("Destroy " << this);
}

LocalVariable::LocalVariable(std::string name) : name(std::move(name)) {
    LOG("Create " << this);
}

LocalVariable::~LocalVariable() {
    LOG("Destroy " << this);
}

TemporaryVariable::TemporaryVariable(int id) : id(id) {
    LOG("Create " << this);
}

TemporaryVariable::~TemporaryVariable() {
    LOG("Destroy " << this);
}

} // namespace qore

#endif // TOOLS_DRIVER_ANALYSIS_H_
