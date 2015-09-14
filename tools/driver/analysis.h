#ifndef TOOLS_DRIVER_ANALYSIS_H_
#define TOOLS_DRIVER_ANALYSIS_H_

#include <cassert>
#include <map>
#include <string>
#include "qore/ast/Visitor.h"
#include "qore/common/Util.h"

namespace qore {

class FunctionAnalyzer;

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

//TODO
    bool isConstant{false};
    mutable void *tag;
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

    const std::string &getValue() const {
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

    operator Storage*() {
        return ptr.get();
    }

private:
    Value(Storage::Ref t) : ptr(t) {
    }

    friend std::ostream &operator<<(std::ostream &os, const Value &value) {
        return os << "value of " << value.ptr;
    }

    friend class FunctionAnalyzer;

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

    operator Storage*() {
        return ptr.get();
    }

private:
    LValue(Storage::Ref t) : ptr(t) {
    }

    friend std::ostream &operator<<(std::ostream &os, const LValue &lvalue) {
        return os << "l-value " << lvalue.ptr;
    }

    friend class FunctionAnalyzer;

private:
    Storage::Ref ptr;
};

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

    Action(Type type, const Storage *s1 = nullptr, const Storage *s2 = nullptr, const Storage *s3 = nullptr) : type(type), s1(s1), s2(s2), s3(s3) {
        LOG("Creating action " << *this);
    }

    //COPY/MOVE
//private:
    Type type;
    const Storage *s1;
    const Storage *s2;
    const Storage *s3;

    friend std::ostream &operator<<(std::ostream &, const Action &);
};

std::ostream &operator<<(std::ostream &os, const Action &action) {
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
    QORE_UNREACHABLE("Invalid action");
}

/**
 * \private
 */
class FunctionProcessor {
public:
    virtual ~FunctionProcessor() {}

    virtual void takeOwnership(Storage *o) = 0;     //TODO handle object ownership
    virtual void processAction(const Action &action) = 0;
};

/**
 * \private
 */
class FunctionAnalyzer {

public:
    FunctionAnalyzer(FunctionProcessor &processor) : processor(processor) {
    }

    void finalize() {
        scope.clear();
        processor.processAction(Action::Return);
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
        return c;
    }

    Value load(LValue &&lvalue) {
        return Value(lvalue.ptr);
    }

    Value assign(LValue &&l, Value &&r) {
        processor.processAction(Action(Action::Assign, l, r));
        return load(std::move(l));
    }

    Value trim(LValue &&l) {
        processor.processAction(Action(Action::Trim, l));
        return load(std::move(l));
    }

    Value add(LValue &&dest, Value &&l, Value &&r) {
        if (!dest) {
            dest = createTemporary();
        }
        processor.processAction(Action(Action::Add, dest, l, r));
        return load(std::move(dest));
    }

    void print(Value &&v) {
        processor.processAction(Action(Action::Print, v));
    }

private:
    Storage::Ref createObject(Storage *s) {
        processor.takeOwnership(s);
        processor.processAction(Action(Action::LifetimeStart, s));
        return std::shared_ptr<Storage>(s, [this](Storage *s){processor.processAction(Action(Action::LifetimeEnd, s));});
    }

    LValue createTemporary() {
        return LValue(createObject(new TemporaryVariable(tempCount++)));
    }

private:
    FunctionProcessor &processor;
    std::map<std::string, LValue> scope;        //TODO destroy in the order opposite of creation
    int tempCount{0};
};

/**
 * \private
 */
LValue evalLValue(FunctionAnalyzer &, const ast::Expression::Ptr &);

/**
 * \private
 */
Value evalValue(FunctionAnalyzer &, const ast::Expression::Ptr &, LValue && = LValue());

/**
 * \private
 */
void eval(FunctionAnalyzer &, const ast::Expression::Ptr &, LValue && = LValue());

/**
 * \private
 */
class LValueExpressionEvaluator : public ast::ExpressionVisitor {

private:
    LValueExpressionEvaluator(FunctionAnalyzer &analyzer) : analyzer(analyzer) {
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
        result = analyzer.createLocalVariable(node->name);
    }

    void visit(const ast::Identifier *node) override {
        result = analyzer.resolveLocalVariable(node->name);
    }

private:
    FunctionAnalyzer &analyzer;
    LValue result;

    friend LValue evalLValue(FunctionAnalyzer &, const ast::Expression::Ptr &);
};

/**
 * \private
 */
class ValueExpressionEvaluator : public ast::ExpressionVisitor {

protected:
    ValueExpressionEvaluator(FunctionAnalyzer &analyzer, bool needsValue, LValue dest) : analyzer(analyzer), needsValue(needsValue), dest(std::move(dest)) {
    }

public:
    void visit(const ast::IntegerLiteral *node) override {
        //TODO
    }

    void visit(const ast::StringLiteral *node) override {
        assign(analyzer.loadConstant(node->value));
        checkNoEffect();
    }

    void visit(const ast::BinaryExpression *node) override {
        Value l = evalValue(analyzer, node->left);
        result = analyzer.add(std::move(dest), std::move(l), evalValue(analyzer, node->right));
        checkNoEffect();
    }

    void visit(const ast::UnaryExpression *node) override {
        assign(analyzer.trim(evalLValue(analyzer, node->operand)));
    }

    void visit(const ast::Assignment *node) override {
        assign(evalValue(analyzer, node->right, evalLValue(analyzer, node->left)));
    }

    void visit(const ast::VarDecl *node) override {
        assign(analyzer.load(analyzer.createLocalVariable(node->name)));
    }

    void visit(const ast::Identifier *node) override {
        assign(analyzer.load(analyzer.resolveLocalVariable(node->name)));
        checkNoEffect();
    }

private:
    void assign(Value &&value) {
        result = !dest ? std::move(value) : analyzer.assign(std::move(dest), std::move(value));
    }

    void checkNoEffect() {
        if (!dest && !needsValue) {
            //error statement has no effect
        }
    }

private:
    FunctionAnalyzer &analyzer;
    bool needsValue;
    LValue dest;
    Value result;

    friend Value evalValue(FunctionAnalyzer &, const ast::Expression::Ptr &, LValue &&);
    friend void eval(FunctionAnalyzer &, const ast::Expression::Ptr &, LValue &&);
};

inline LValue evalLValue(FunctionAnalyzer &analyzer, const ast::Expression::Ptr &node) {
    LValueExpressionEvaluator r(analyzer);
    node->accept(r);
    return std::move(r.result);
}

inline Value evalValue(FunctionAnalyzer &analyzer, const ast::Expression::Ptr &node, LValue &&dest) {
    ValueExpressionEvaluator r(analyzer, true, std::move(dest));
    node->accept(r);
    return std::move(r.result);
}

inline void eval(FunctionAnalyzer &analyzer, const ast::Expression::Ptr &node, LValue &&dest) {
    ValueExpressionEvaluator r(analyzer, false, std::move(dest));
    node->accept(r);
}

/**
 * \private
 */
class StatementAnalyzer : public ast::StatementVisitor, public ast::ProgramVisitor {

public:
    StatementAnalyzer(FunctionProcessor &processor) : analyzer(processor) {
    }

    void visit(const ast::EmptyStatement *node) override {
        LOG_FUNCTION();
    }

    void visit(const ast::ExpressionStatement *node) override {
        LOG_FUNCTION();
        eval(analyzer, node->expression);
    }

    void visit(const ast::PrintStatement *node) override {
        LOG_FUNCTION();
        analyzer.print(evalValue(analyzer, node->expression));
    }

    void visit(const ast::Program *node) override {
        for (const auto &stmt : node->body) {
            stmt->accept(*this);
        }
        analyzer.finalize();
    }

private:
    FunctionAnalyzer analyzer;
};

Constant::Constant(std::string value) : value(std::move(value)) {
    LOG("Create " << this);
    isConstant = true;
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

class Function {

public:
    void dump() const {
        LOG_FUNCTION();
        for (const Action &a : actions) {
            LOG(a);
        }
    }

    //COPY/MOVE
//private:
    std::vector<std::unique_ptr<Storage>> objects;
    std::vector<Action> actions;

    friend class CImlp;
};

struct FunctionBuilder : public FunctionProcessor {

    FunctionBuilder() : f(new Function()) {
    }

    void takeOwnership(Storage *s) override {
        f->objects.emplace_back(s);
    }

    void processAction(const Action &action) override {
        f->actions.push_back(action);
    }

    std::unique_ptr<Function> build() {
        return std::move(f);
    }

private:
    std::unique_ptr<Function> f;
};

} // namespace qore

#endif // TOOLS_DRIVER_ANALYSIS_H_
