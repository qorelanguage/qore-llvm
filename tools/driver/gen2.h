#ifndef TOOLS_DRIVER_GEN2_H_
#define TOOLS_DRIVER_GEN2_H_

#include "qore/common/Logging.h"

#include <map>
#include <string>
#include "qore/ast/Program.h"
#include "qore/common/Util.h"

#include "llvm/ADT/APSInt.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/raw_os_ostream.h"

namespace qore {

class QValue {
public:
    explicit QValue(llvm::Value *value = nullptr) : value(value) {
    }

    ~QValue() {
        assert(!value);
    }

    QValue(QValue &&src) : value(src.value) {
        src.value = nullptr;
    }

    QValue &operator=(QValue &&src) {
        assert(!value);
        value = src.value;
        src.value = nullptr;
        return *this;
    }

    operator bool() {
        return value != nullptr;
    }

    void push(llvm::IRBuilder<> &builder, std::vector<llvm::Value *> &args) {
        assert(value);
        args.push_back(builder.CreateExtractValue(value, {0}));
        args.push_back(builder.CreateExtractValue(value, {1}));
    }

    using Type = llvm::Value *;
    llvm::Value *release() {
        assert(value);
        llvm::Value *v = value;
        value = nullptr;
        return v;
    }

    QValue clone() {
        assert(value);
        return QValue(value);
    }

    void doSwap(llvm::IRBuilder<> &builder, llvm::Value *dest, llvm::Value *orig) {
        assert(value && orig);
        builder.CreateStore(value, dest);
        value = orig;
    }

    void cleanup(class CodeGen &);

private:
    QValue(const QValue &) = delete;
    QValue &operator=(const QValue &) = delete;

private:
    llvm::Value *value;
};

class QLValue {
public:
    explicit QLValue(ast::Variable *var = nullptr) : var(var) {
    }

    ~QLValue() {
        assert(!var);
    }

    QLValue(QLValue &&src) : var(src.var) {
        src.var = nullptr;
    }

    QLValue &operator=(QLValue &&src) {
        assert(!var);
        var = src.var;
        src.var = nullptr;
        return *this;
    }

    operator bool() {
        return var != nullptr;
    }

    void swapValue(llvm::IRBuilder<> &builder, QValue &src) {
        llvm::Value *old = builder.CreateLoad(ptr(), "old." + var->name);
        src.doSwap(builder, ptr(), old);
    }

    void push(llvm::IRBuilder<> &builder, std::vector<llvm::Value *> &args) {
        assert(var);
        args.push_back(ptr());
    }

    using Type = void;

    void release() {
        var = nullptr;
    }

    void cleanup(class CodeGen &);

private:
    llvm::Value *ptr() {
        assert(var);
        return  static_cast<llvm::Value *>(var->data);
    }

private:
    QLValue(const QLValue &) = delete;
    QLValue &operator=(const QLValue &) = delete;

private:
    ast::Variable *var;
};

class Cleanup {
public:
    Cleanup() : cg(nullptr), lVal(nullptr) {
    }

    void buildLPad();

    void add(QValue *v) {
        vals.push_back(v);
    }

    void add(QLValue *v) {
        assert(!lVal);
        lVal = v;
    }

    llvm::Value *remove(QValue *v) {
        auto it = std::find(vals.begin(), vals.end(), v);
        assert(it != vals.end());
        vals.erase(it);
        return v->release();
    }

    void remove(QLValue *v) {
        assert(lVal == v);
        lVal = nullptr;
        v->release();
    }

private:
    class CodeGen *cg;
    QLValue *lVal;
    std::vector<QValue *> vals;

    friend class CodeGen;
};



/**
 * \private
 */
template<typename T>
class CallBuilderBase {

public:
    CallBuilderBase(llvm::IRBuilder<> &builder, llvm::Function *f) : builder(builder), f(f), buildCalled(false) {
    }

    virtual ~CallBuilderBase() {
        assert(buildCalled);
    }

public:
    T &withGenericArg(llvm::Value *v) {
        args.push_back(v);
        return *static_cast<T*>(this);
    }

    T &withIntArg(int64_t value) {
        args.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(builder.getContext()), value));
        return *this;
    }

//    T &withQoreValuePtrArg(llvm::Value *v) {
//        args.push_back(v);
//        return *static_cast<T*>(this);
//    }

    T &withQoreValuePtrArg(QLValue &v) {
        v.push(builder, args);
        return *static_cast<T*>(this);
    }

    T &withQoreValueArg(QValue &v) {
        v.push(builder, args);
        return *static_cast<T*>(this);
    }

    T &withQoreValueArg2(llvm::Value *v) {
        args.push_back(builder.CreateExtractValue(v, {0}));
        args.push_back(builder.CreateExtractValue(v, {1}));
        return *static_cast<T*>(this);
    }

    llvm::Value *build() {
        assert(!buildCalled);
        buildCalled = true;
        return doBuild(builder, f, args);
    }

protected:
    virtual llvm::Value *doBuild(llvm::IRBuilder<> &builder, llvm::Function *f, std::vector<llvm::Value *> &args) {
        return builder.CreateCall(f, args);
    }

protected:
    llvm::IRBuilder<> &builder;
    llvm::Function *f;
    std::vector<llvm::Value *> args;

private:
    bool buildCalled;
};

class CallBuilder : public CallBuilderBase<CallBuilder> {
public:
    CallBuilder(llvm::IRBuilder<> &builder, llvm::Function *f) : CallBuilderBase(builder, f) {
    }
};

/**
 * \private
 */
class InvokeBuilder : public CallBuilderBase<InvokeBuilder> {

public:
    InvokeBuilder(const std::string &name, llvm::IRBuilder<> &builder, llvm::Function *f, llvm::BasicBlock *cb, Cleanup &cleanup) : CallBuilderBase(builder, f), name(name), cb(cb), cleanup(cleanup) {
    }

protected:
    llvm::Value *doBuild(llvm::IRBuilder<> &builder, llvm::Function *f, std::vector<llvm::Value *> &args) override {
        llvm::Function *parent = builder.GetInsertBlock()->getParent();
        llvm::BasicBlock *bb1 = llvm::BasicBlock::Create(builder.getContext(), name + ".cont", parent);
        llvm::BasicBlock *bb2 = llvm::BasicBlock::Create(builder.getContext(), name + ".lpad", parent);
        llvm::Value *v = builder.CreateInvoke(f, bb1, bb2, args);
        builder.SetInsertPoint(bb2);
        cleanup.buildLPad();
        builder.CreateBr(cb);
        builder.SetInsertPoint(bb1);
        return v;
    }

private:
    const std::string &name;
    llvm::BasicBlock *cb;
    Cleanup &cleanup;
};

class LValEval : public ast::ExpressionVisitor {
public:
    LValEval(llvm::IRBuilder<> &builder, QLValue &result) : builder(builder), result(result) {
    }

    void visit(ast::IntegerLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::StringLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::BinaryExpression::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::UnaryExpression::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::Assignment::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::VarDecl::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::Identifier::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::StringConstant::Ptr node) override {QORE_UNREACHABLE("Not implemented");}

    void visit(ast::Variable::Ptr node) override {
        result = QLValue(node.get());
    }

private:
    llvm::IRBuilder<> &builder;
    QLValue &result;
};









/**
 * \private
 */
class LLVMHelper {
public:
    LLVMHelper() {
        ltVoid = llvm::Type::getVoidTy(ctx);
        ltVoidPtr = llvm::Type::getInt8PtrTy(ctx);
        ltInt32 = llvm::Type::getInt32Ty(ctx);
        ltInt64 = llvm::Type::getInt64Ty(ctx);
        ltUInt8 = llvm::Type::getInt8Ty(ctx);
        ltCharPtr = llvm::Type::getInt8PtrTy(ctx);
        ltQoreValue = llvm::StructType::get(ltInt64, ltUInt8, nullptr);
        ltQoreValuePtr = ltQoreValue->getPointerTo();
        ltBool = llvm::Type::getInt1Ty(ctx);
        ltExc = llvm::StructType::get(ltCharPtr, ltInt32, nullptr);

        fnPrintQv = f(ltVoid, "print_qv", ltInt64, ltUInt8, nullptr);
        fnMakeStr = f(ltQoreValue, "make_str", ltCharPtr, nullptr);
        fnEvalAdd = f(ltQoreValue, "eval_add", ltInt64, ltUInt8, ltInt64, ltUInt8, nullptr);
        fnEvalTrim = f(ltVoid, "eval_trim", ltInt64, ltUInt8, nullptr);
        fnStrongDeref = f(ltVoid, "strongDeref", ltInt64, ltUInt8, nullptr);
        fnStrongDerefNothrow = f(ltVoid, "strongDerefNothrow", ltInt64, ltUInt8, nullptr);
        fnUnlockNothrow = f(ltVoid, "unlock", ltQoreValuePtr, nullptr);
        fnStrongRef = f(ltVoid, "strongRef", ltInt64, ltUInt8, nullptr);
        fnEvalCond = f(ltBool, "eval_cond", ltInt64, ltUInt8, nullptr);

        fnLoadPtr = f(ltQoreValue, "loadPtr", ltQoreValuePtr, nullptr);
        fnReplace = f(ltQoreValue, "replace", ltQoreValuePtr, ltInt64, ltUInt8, nullptr);
        fnLoadUnique = f(ltQoreValue, "load_unique", ltQoreValuePtr, nullptr);

        nothing = llvm::ConstantStruct::get(ltQoreValue,
                llvm::ConstantInt::get(ltInt64, 0),
                llvm::ConstantInt::get(ltUInt8, 0),
                nullptr);
        scope = nullptr;

        ltVoidFunc = llvm::FunctionType::get(ltVoid, false);

        fnPersonality = llvm::Function::Create(x(ltInt32, true, nullptr), llvm::Function::ExternalLinkage, "__gxx_personality_v0", module);

        fnStrongDeref->addFnAttr(llvm::Attribute::NoUnwind);
        fnStrongDeref->addFnAttr(llvm::Attribute::UWTable);
        qMain = llvm::Function::Create(ltVoidFunc, llvm::Function::ExternalLinkage, "q", module);
    }

    CallBuilder call(llvm::Function *f) {
        return CallBuilder(builder, f);
    }

    InvokeBuilder invoke(const std::string &name, llvm::Function *f) {
        return InvokeBuilder(name, builder, f, getCatch(), cleanup);
    }

    llvm::BasicBlock *getCatch() {
        if (exceptionBlocks.empty()) {
            llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "resume", qMain);
            llvm::IRBuilder<> bld(ctx);
            bld.SetInsertPoint(bb);
            llvm::Value *ex = bld.CreateLoad(excSlot);
            bld.CreateResume(ex);
            exceptionBlocks.push_back(bb);
        }
        return exceptionBlocks.back();
    }

    std::vector<llvm::BasicBlock *> exceptionBlocks;

    void location(const qore::SourceLocation &l) {
//        builder.SetCurrentDebugLocation(llvm::DILocation::get(ctx, l.line, l.column, scope));
    }

    void location(const qore::SourceRange &r) {
        location(r.start);
    }

    llvm::LLVMContext &ctx{llvm::getGlobalContext()};
    llvm::Module *module{new llvm::Module("Q", ctx)};
    llvm::DIBuilder dib{*module};
    llvm::IRBuilder<> builder{ctx};

    llvm::Constant *nothing;
    llvm::Type *ltVoid;
    llvm::Type *ltVoidPtr;
    llvm::Type *ltUInt8;
    llvm::Type *ltInt32;
    llvm::Type *ltInt64;
    llvm::Type *ltCharPtr;
    llvm::Type *ltBool;
    llvm::StructType *ltQoreValue;
    llvm::StructType *ltExc;
    llvm::Type *ltQoreValuePtr;
    llvm::Function *fnPrintQv;
    llvm::Function *fnMakeStr;
    llvm::Function *fnEvalAdd;
    llvm::Function *fnEvalTrim;
    llvm::Function *fnEvalCond;
    llvm::Function *fnStrongDeref;
    llvm::Function *fnStrongDerefNothrow;
    llvm::Function *fnUnlockNothrow;
    llvm::Function *fnStrongRef;
    llvm::Function *fnLoadPtr;
    llvm::Function *fnReplace;
    llvm::Function *fnLoadUnique;
    llvm::Function *fnPersonality;
    llvm::Value *excSlot;

    llvm::FunctionType *ltVoidFunc;

    llvm::DISubprogram *scope;

    llvm::DIFile *diFile;
    llvm::DICompositeType *ditQoreValue;
    llvm::Function *qMain;

    void globalXtor(const char *globalName, const char *funcName, llvm::IRBuilder<> &xtorBuilder) {
        llvm::Type *funcTypePtr = llvm::PointerType::getUnqual(ltVoidFunc);
        llvm::StructType *structType = llvm::StructType::get(ltInt32, funcTypePtr, ltVoidPtr, nullptr);
        llvm::ArrayType *structArrayType = llvm::ArrayType::get(structType, 1);
        llvm::Function *func = llvm::Function::Create(ltVoidFunc, llvm::Function::InternalLinkage, funcName, module);
        llvm::Constant *structArray[] = {
                llvm::ConstantStruct::get(structType,
                        llvm::ConstantInt::get(ltInt32, 65535, false),
                        llvm::ConstantExpr::getBitCast(func, funcTypePtr),
                        llvm::Constant::getNullValue(ltVoidPtr),
                        nullptr)
        };
        new llvm::GlobalVariable(*module, structArrayType, false,
                llvm::GlobalValue::AppendingLinkage,
                llvm::ConstantArray::get(structArrayType, structArray),
                globalName);
        xtorBuilder.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", func));
        xtorBuilder.SetInsertPoint(xtorBuilder.CreateRetVoid());
    }

    llvm::GlobalVariable *createStringConstant(const std::string &value) {
        llvm::Constant *val = llvm::ConstantDataArray::getString(ctx, value, true);
        llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(*module, val->getType(), true,
                llvm::GlobalValue::PrivateLinkage, val);
        globalVar->setUnnamedAddr(true);
        return globalVar;
    }

private:
    llvm::FunctionType *xv(llvm::Type *returnType, bool varArg, va_list v) {
        std::vector<llvm::Type *> args;
        while (true) {
            llvm::Type *arg = va_arg(v, llvm::Type*);
            if (arg == nullptr) {
                break;
            }
            args.push_back(arg);
        }
        return llvm::FunctionType::get(returnType, args, varArg);
    }

    llvm::FunctionType *x(llvm::Type *returnType, bool varArg, ...) {
        va_list v;
        va_start(v, varArg);
        llvm::FunctionType *ft = xv(returnType, varArg, v);
        va_end(v);
        return ft;
    }

    llvm::Function *f(llvm::Type *returnType, const std::string &name, ...) {
        va_list v;
        va_start(v, name);
        llvm::FunctionType *ft = xv(returnType, false, v);
        va_end(v);
        return llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, module);
    }

public:
    Cleanup cleanup;

public:
    void discard(QValue &v) {
        llvm::Value *vv = cleanup.remove(&v);
        invoke("deref", fnStrongDeref).withQoreValueArg2(vv).build();
    }

    void release(QLValue &v) {
        //unlock
        cleanup.remove(&v);
    }
};

class CodeGen;
void eval(CodeGen &codeGen, ast::Expression::Ptr &node, QValue &dest);
void evalLValue(CodeGen &codeGen, ast::Expression::Ptr &node, QLValue &dest);

class CodeGen : public LLVMHelper, private ast::StatementVisitor {

public:
    CodeGen(Script &script) : script(script) {
        llvm::IRBuilder<> ctorBuilder{ctx};
        llvm::IRBuilder<> dtorBuilder{ctx};
        globalXtor("llvm.global_ctors", "qCtor", ctorBuilder);
        globalXtor("llvm.global_dtors", "qDtor", dtorBuilder);

        for (auto &s : script.strings) {
            llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(*module, ltQoreValue, false,
                    llvm::GlobalValue::PrivateLinkage, nothing);    //XXX these should be QoreObjects (with implicit Tag::Str)
            globalVar->setUnnamedAddr(true);

            //ctor
            llvm::Value *strPtr = ctorBuilder.CreateConstGEP2_32(nullptr, createStringConstant(s->value), 0, 0);
            ctorBuilder.CreateStore(CallBuilder(ctorBuilder, fnMakeStr).withGenericArg(strPtr).build(), globalVar);

            //dtor
            llvm::LoadInst *val = dtorBuilder.CreateLoad(globalVar);
            CallBuilder(dtorBuilder, fnStrongDeref).withQoreValueArg2(val).build();
            dtorBuilder.SetInsertPoint(val);

            s->data = globalVar;
        }

        qMain->setPersonalityFn(llvm::ConstantExpr::getBitCast(fnPersonality, ltCharPtr));
        builder.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", qMain));

        excSlot = builder.CreateAlloca(ltExc, nullptr, "exc.slot");

        for (auto &v : script.variables) {
            llvm::AllocaInst *var = builder.CreateAlloca(ltQoreValue, nullptr, v->name);
            builder.CreateStore(nothing, var);
            v->data = var;
        }
        cleanup.cg = this;
    }

    void run() {
        script.body->accept(*this);
        builder.CreateRetVoid();
    }

    llvm::Module *getModule() {
//        qMain->viewCFG();
//        module->dump();
        llvm::raw_os_ostream sss(std::cout);
        llvm::verifyModule(*module, &sss);
        return module;
    }

private:
    void visit(ast::EmptyStatement::Ptr node) override {
    }

    void visit(ast::IfStatement::Ptr node) override {
        QValue cond;
        eval(node->condition, cond);
        llvm::Value *b = call(fnEvalCond).withQoreValueArg(cond).build();   //can it throw?
        discard(cond);

        llvm::BasicBlock *bb1 = llvm::BasicBlock::Create(ctx, "if.then", qMain);
        llvm::BasicBlock *bb2 = llvm::BasicBlock::Create(ctx, "if.else", qMain);
        llvm::BasicBlock *bb3 = llvm::BasicBlock::Create(ctx, "if.cont", qMain);
        builder.CreateCondBr(b, bb1, bb2);

        builder.SetInsertPoint(bb1);
        node->thenBranch->accept(*this);
        builder.CreateBr(bb3);

        builder.SetInsertPoint(bb2);
        node->elseBranch->accept(*this);
        builder.CreateBr(bb3);

        builder.SetInsertPoint(bb3);
    }

    void visit(ast::TryStatement::Ptr node) override {
        llvm::BasicBlock *bb1 = llvm::BasicBlock::Create(ctx, "try.cont", qMain);
        llvm::BasicBlock *bb2 = llvm::BasicBlock::Create(ctx, "catch", qMain);
        exceptionBlocks.push_back(bb2);
        node->tryBody->accept(*this);
        builder.CreateBr(bb1);
        exceptionBlocks.pop_back();

        builder.SetInsertPoint(bb2);
        node->catchBody->accept(*this);
        builder.CreateBr(bb1);

        builder.SetInsertPoint(bb1);
    }

    void visit(ast::PrintStatement::Ptr node) override {
        QValue val;
        eval(node->expression, val);
        call(fnPrintQv).withQoreValueArg(val).build();      //print_qv is noexcept
        discard(val);
    }

    void visit(ast::ExpressionStatement::Ptr node) override {
        QValue val;
        eval(node->expression, val);
        //this will not be needed
        discard(val);
    }

    void visit(ast::CompoundStatement::Ptr node) override {
        for (auto &stmt : node->statements) {
            stmt->accept(*this);
        }
    }

    void visit(ast::ScopedStatement::Ptr node) override {
        if (node->variables.empty()) {
            node->statement->accept(*this);
        } else {
            llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "scope.cleanup", qMain);
            exceptionBlocks.push_back(bb);
//            for (auto &v : node->variables) {
//                //declare + lifetime.start
//            }

            node->statement->accept(*this);

            exceptionBlocks.pop_back();

            llvm::IRBuilder<> bld(ctx);
            bld.SetInsertPoint(bb);

            std::vector<QValue> vvv(node->variables.size());
            int i = 0;
            for (auto &v : node->variables) {
                llvm::Value *var = static_cast<llvm::Value *>(v->data);
                llvm::Value *val = builder.CreateLoad(var);
                vvv[i] = QValue(val);
                cleanup.add(&vvv[i++]);
                CallBuilder(bld, fnStrongDerefNothrow).withQoreValueArg2(bld.CreateLoad(var)).build();
            }

            for (i = vvv.size() - 1; i >= 0; --i) {
                llvm::Value *val = cleanup.remove(&vvv[i]);
                invoke("deref", fnStrongDeref).withQoreValueArg2(val).build();
            }
            bld.CreateBr(getCatch());
        }
    }

    void eval(ast::Expression::Ptr &node, QValue &dest) {
        return ::qore::eval(*this, node, dest);
    }

private:
    Script &script;
};



class ValEval : public ast::ExpressionVisitor {

public:
    ValEval(CodeGen &codeGen, QValue &result) : codeGen(codeGen), result(result) {
    }

    void visit(ast::IntegerLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::StringLiteral::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::VarDecl::Ptr node) override {QORE_UNREACHABLE("Not implemented");}
    void visit(ast::Identifier::Ptr node) override {QORE_UNREACHABLE("Not implemented");}

    void visit(ast::StringConstant::Ptr node) override {
        llvm::Value *s = static_cast<llvm::Value *>(node->data);
        setResult(codeGen.builder.CreateLoad(s, "str." + node->value));
    }

    void visit(ast::Variable::Ptr node) override {
        llvm::Value *v = static_cast<llvm::Value *>(node->data);
        setResult(codeGen.builder.CreateLoad(v, "val." + node->name));
    }

    void visit(ast::BinaryExpression::Ptr node) override {
        QValue left;
        QValue right;
        eval(node->left, left);
        eval(node->right, right);
        result = QValue(codeGen.invoke("add", codeGen.fnEvalAdd).withQoreValueArg(left).withQoreValueArg(right).build());
        codeGen.cleanup.add(&result);
        codeGen.discard(right);
        codeGen.discard(left);
    }

    void visit(ast::UnaryExpression::Ptr node) override {
        QLValue lval;
        evalLValue(node->operand, lval);

        QValue val = QValue(codeGen.invoke("unique", codeGen.fnLoadUnique).withQoreValuePtrArg(lval).build());
        codeGen.cleanup.add(&val);
        codeGen.invoke("trim", codeGen.fnEvalTrim).withQoreValueArg(val).build();
        setResult(val);      //if needs value, add cleanup of curVal
        lval.swapValue(codeGen.builder, val);
        codeGen.release(lval);
        codeGen.discard(val);
    }

    void visit(ast::Assignment::Ptr node) override {
        QLValue left;
        QValue right;
        eval(node->right, right);
        evalLValue(node->left, left);
        //type conversion
        setResult(right);      //if needs value
        left.swapValue(codeGen.builder, right);
        codeGen.release(left);
        codeGen.discard(right);
    }

    void setResult(QValue &v) {
        assert(!result);
        codeGen.call(codeGen.fnStrongRef).withQoreValueArg(v).build();
        result = v.clone();
        codeGen.cleanup.add(&result);
    }

    void setResult(llvm::Value *v) {
        assert(!result);
        codeGen.call(codeGen.fnStrongRef).withQoreValueArg2(v).build();
        result = QValue(v);
        codeGen.cleanup.add(&result);
    }

    void eval(ast::Expression::Ptr &node, QValue &dest) {
        ::qore::eval(codeGen, node, dest);
    }

    void evalLValue(ast::Expression::Ptr &node, QLValue &dest) {
        ::qore::evalLValue(codeGen, node, dest);
    }

private:
    CodeGen &codeGen;
    QValue &result;
};

void eval(CodeGen &codeGen, ast::Expression::Ptr &node, QValue &dest) {
    LOG_FUNCTION();
    ValEval v(codeGen, dest);
    node->accept(v);
}

void evalLValue(CodeGen &codeGen, ast::Expression::Ptr &node, QLValue &dest) {
    LOG_FUNCTION();
    LValEval v(codeGen.builder, dest);
    node->accept(v);
    codeGen.cleanup.add(&dest);
}

void QValue::cleanup(class CodeGen &cg) {
    cg.call(cg.fnStrongDerefNothrow).withQoreValueArg2(value).build();
}

void QLValue::cleanup(class CodeGen &cg) {
    cg.call(cg.fnUnlockNothrow).withQoreValuePtrArg(*this).build();
}

void Cleanup::buildLPad() {
    LOG_FUNCTION();
    llvm::LandingPadInst *l = cg->builder.CreateLandingPad(cg->ltExc, 1);
    l->setCleanup(true);
    cg->builder.CreateStore(l, cg->excSlot);
    if (lVal) {
        lVal->cleanup(*cg);
    }
    for (auto it = vals.rbegin(); it != vals.rend(); ++it) {
        (*it)->cleanup(*cg);
    }
}

std::unique_ptr<llvm::Module> doCodeGen(Script &script) {
    CodeGen cg(script);
    cg.run();
    return std::unique_ptr<llvm::Module>(cg.getModule());
}

} // namespace qore

#endif // TOOLS_DRIVER_GEN2_H_
//TODO LLVM mustn't reorder things!  -> volatile
