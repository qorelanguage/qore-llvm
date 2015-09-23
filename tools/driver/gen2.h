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

/**
 * \private
 */
class CallBuilder {

public:
    CallBuilder(llvm::IRBuilder<> &builder, llvm::Function *f) : builder(builder), f(f), buildCalled(false) {
    }

    ~CallBuilder() {
        assert(buildCalled);
    }

public:
    CallBuilder &withGenericArg(llvm::Value *v) {
        args.push_back(v);
        return *this;
    }

    CallBuilder &withIntArg(int64_t value) {
        args.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(builder.getContext()), value));
        return *this;
    }

    CallBuilder &withQoreValuePtrArg(llvm::Value * v) {
        args.push_back(v);
        return *this;
    }

    CallBuilder &withQoreValueArg(llvm::Value * v) {
        args.push_back(builder.CreateExtractValue(v, {0}));
        args.push_back(builder.CreateExtractValue(v, {1}));
        return *this;
    }

    llvm::Value *build() {
        assert(!buildCalled);
        buildCalled = true;
        return builder.CreateCall(f, args);
    }

private:
    llvm::IRBuilder<> &builder;
    llvm::Function *f;
    std::vector<llvm::Value *> args;
    bool buildCalled;
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

        fnPrintQv = f(ltVoid, "print_qv", ltInt64, ltUInt8, nullptr);
        fnMakeStr = f(ltQoreValue, "make_str", ltCharPtr, nullptr);
        fnEvalAdd = f(ltQoreValue, "eval_add", ltInt64, ltUInt8, ltInt64, ltUInt8, nullptr);
        fnEvalTrim = f(ltVoid, "eval_trim", ltInt64, ltUInt8, nullptr);
        fnStrongDeref = f(ltVoid, "strongDeref", ltInt64, ltUInt8, nullptr);
        fnStrongRef = f(ltVoid, "strongRef", ltInt64, ltUInt8, nullptr);

        fnLoadPtr = f(ltQoreValue, "loadPtr", ltQoreValuePtr, nullptr);
        fnReplace = f(ltQoreValue, "replace", ltQoreValuePtr, ltInt64, ltUInt8, nullptr);
        fnLoadUnique = f(ltQoreValue, "load_unique", ltQoreValuePtr, nullptr);

        nothing = llvm::ConstantStruct::get(ltQoreValue,
                llvm::ConstantInt::get(ltInt64, 0),
                llvm::ConstantInt::get(ltUInt8, 0),
                nullptr);
        scope = nullptr;

        ltVoidFunc = llvm::FunctionType::get(ltVoid, false);
        llvm::Function *f = llvm::Function::Create(ltVoidFunc, llvm::Function::ExternalLinkage, "q", module);
        builder.SetInsertPoint(llvm::BasicBlock::Create(ctx, "entry", f));
    }

    CallBuilder call(llvm::Function *f) {
        return CallBuilder(builder, f);
    }

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
    llvm::StructType *ltQoreValue;
    llvm::Type *ltQoreValuePtr;
    llvm::Function *fnPrintQv;
    llvm::Function *fnMakeStr;
    llvm::Function *fnEvalAdd;
    llvm::Function *fnEvalTrim;
    llvm::Function *fnStrongDeref;
    llvm::Function *fnStrongRef;
    llvm::Function *fnLoadPtr;
    llvm::Function *fnReplace;
    llvm::Function *fnLoadUnique;

    llvm::FunctionType *ltVoidFunc;

    llvm::DISubprogram *scope;

    llvm::DIFile *diFile;
    llvm::DICompositeType *ditQoreValue;


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
    llvm::FunctionType *x(llvm::Type *returnType, va_list v) {
        std::vector<llvm::Type *> args;
        while (true) {
            llvm::Type *arg = va_arg(v, llvm::Type*);
            if (arg == nullptr) {
                break;
            }
            args.push_back(arg);
        }
        return llvm::FunctionType::get(returnType, args, false);
    }

    llvm::FunctionType *x(llvm::Type *returnType, ...) {
        va_list v;
        va_start(v, returnType);
        llvm::FunctionType *ft = x(returnType, v);
        va_end(v);
        return ft;
    }

    llvm::Function *f(llvm::Type *returnType, const std::string &name, ...) {
        va_list v;
        va_start(v, name);
        llvm::FunctionType *ft = x(returnType, v);
        va_end(v);
        return llvm::Function::Create(ft, llvm::Function::ExternalLinkage, name, module);
    }
};

/**
 * \private
 */
class NewBackend : public LLVMHelper {
public:
    using StringLiteralData = llvm::GlobalVariable *;
    using VariableData = llvm::AllocaInst *;
    using Value = llvm::Value *;
    using LValue = llvm::Value *;

    NewBackend() {
        globalXtor("llvm.global_ctors", "qCtor", ctorBuilder);
        globalXtor("llvm.global_dtors", "qDtor", dtorBuilder);
    }

    VariableData createVariable(const std::string &name) {
        llvm::AllocaInst *var = builder.CreateAlloca(ltQoreValue, nullptr, name);
        builder.CreateStore(nothing, var);
        return var;
    }

    void destroyVariable(const VariableData &d) {
    }

    StringLiteralData createStringLiteral(const std::string &value) {
        llvm::GlobalVariable *globalVar = new llvm::GlobalVariable(*module, ltQoreValue, false,
                llvm::GlobalValue::PrivateLinkage, nothing);
        globalVar->setUnnamedAddr(true);

        //ctor
        llvm::Value *strPtr = ctorBuilder.CreateConstGEP2_32(nullptr, createStringConstant(value), 0, 0);
        ctorBuilder.CreateStore(CallBuilder(ctorBuilder, fnMakeStr).withGenericArg(strPtr).build(), globalVar);

        //dtor
        llvm::LoadInst *val = dtorBuilder.CreateLoad(globalVar);
        CallBuilder(dtorBuilder, fnStrongDeref).withQoreValueArg(val).build();
        dtorBuilder.SetInsertPoint(val);

        return globalVar;
    }

    void destroyStringLiteral(const StringLiteralData &) {
    }

    Value add(const Value &l, const Value &r) {
        return call(fnEvalAdd).withQoreValueArg(l).withQoreValueArg(r).build();
    }

    Value assign(const LValue &dest, const Value &newValue) {
        llvm::Value *originalValue = builder.CreateLoad(dest);
        builder.CreateStore(newValue, dest);
        return originalValue;
    }

    Value getNothing() {
        return nothing;
    }

    void lifetimeEnd(const VariableData &var) {
        //lifetime end
    }

    void lifetimeStart(const VariableData &var) {
        //lifetime start
    }

    Value loadStringLiteralValue(const StringLiteralData &str) {
        return builder.CreateLoad(str);         //TODO align, volatile
    }

    Value loadUnique(const LValue &lval) {
        return call(fnLoadUnique).withQoreValuePtrArg(lval).build();
    }

    Value loadVariableValue(const VariableData &var) {
        return builder.CreateLoad(var);         //TODO align, volatile
    }

    LValue loadVarPtr(const VariableData &var) {
        return var;
    }

    void print(const Value &v) {
        call(fnPrintQv).withQoreValueArg(v).build();
    }

    void strongDeref(Value v) {
        call(fnStrongDeref).withQoreValueArg(v).build();
    }

    void strongRef(Value v) {
        call(fnStrongRef).withQoreValueArg(v).build();
    }

    void trim(const Value &v) {
        call(fnEvalTrim).withQoreValueArg(v).build();
    }

    void setLocation(const SourceLocation &loc) {
        location(loc);
    }

    llvm::Module *getModule() {
        builder.CreateRetVoid();
        module->dump();
        return module;
    }

private:
    llvm::IRBuilder<> ctorBuilder{ctx};
    llvm::IRBuilder<> dtorBuilder{ctx};
};

std::unique_ptr<llvm::Module> doCodeGen(const qore::qil::Script &script) {
    NewBackend cg;
    Runner<NewBackend> r(script, cg);
    r.run();
    return std::unique_ptr<llvm::Module>(cg.getModule());
}

} // namespace qore

#endif // TOOLS_DRIVER_GEN2_H_
//TODO LLVM mustn't reorder things!  -> volatile
