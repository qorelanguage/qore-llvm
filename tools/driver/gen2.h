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

    operator llvm::Value*() {
        return build();
    }

private:
    llvm::IRBuilder<> &builder;
    llvm::Function *f;
    std::vector<llvm::Value *> args;
    bool buildCalled;
};


class LLVMHelper {
public:
//    CodeGen(qore::SourceManager &sourceMgr) : sourceMgr(sourceMgr) {
    LLVMHelper() {
        ltVoid = llvm::Type::getVoidTy(ctx);
        ltVoidPtr = llvm::Type::getInt8PtrTy(ctx);
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
        fnStrongDerefPtr = f(ltVoid, "strongDerefPtr", ltQoreValuePtr, nullptr);
        fnStrongRef = f(ltVoid, "strongRef", ltInt64, ltUInt8, nullptr);

        fnLoadPtr = f(ltQoreValue, "loadPtr", ltQoreValuePtr, nullptr);
        fnReplace = f(ltQoreValue, "replace", ltQoreValuePtr, ltInt64, ltUInt8, nullptr);
        fnLoadUnique = f(ltQoreValue, "load_unique", ltQoreValuePtr, nullptr);

        nothing = llvm::ConstantStruct::get(ltQoreValue, llvm::ConstantInt::get(ltInt64, 0), llvm::ConstantInt::get(ltUInt8, 0), nullptr);
        scope = nullptr;

        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "q", module);
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "entry", f);
        builder.SetInsertPoint(bb);
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
    llvm::Type *ltInt64;
    llvm::Type *ltCharPtr;
    llvm::StructType *ltQoreValue;
    llvm::Type *ltQoreValuePtr;
    llvm::Function *fnPrintQv;
    llvm::Function *fnMakeStr;
    llvm::Function *fnEvalAdd;
    llvm::Function *fnEvalTrim;
    llvm::Function *fnStrongDeref;
    llvm::Function *fnStrongDerefPtr;
    llvm::Function *fnStrongRef;
    llvm::Function *fnLoadPtr;
    llvm::Function *fnReplace;
    llvm::Function *fnLoadUnique;

    llvm::DISubprogram *scope;

    llvm::DIFile *diFile;
    llvm::DICompositeType *ditQoreValue;

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

class NewBackend : public LLVMHelper {
public:
    using StringLiteralData = llvm::GlobalVariable *;
    using LocalVariableData = llvm::AllocaInst *;
    using Value = llvm::Value *;
    using LValue = llvm::Value *;

    NewBackend() {
        llvm::FunctionType* CtorFTy;
        llvm::Type *CtorPFTy;
        llvm::StructType *CtorStructTy;


        CtorFTy = llvm::FunctionType::get(ltVoid, false);
        CtorPFTy = llvm::PointerType::getUnqual(CtorFTy);

        // Get the type of a ctor entry, { i32, void ()*, i8* }.
        CtorStructTy = llvm::StructType::get(llvm::Type::getInt32Ty(ctx), llvm::PointerType::getUnqual(CtorFTy), ltVoidPtr, nullptr);

        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::InternalLinkage, "qInit", module);
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "entry", f);
        ctorBuilder.SetInsertPoint(bb);
        ctorBuilder.SetInsertPoint(ctorBuilder.CreateRetVoid());


        // Construct the constructor and destructor arrays.
          llvm::Constant *S[] = {
              llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 65535, false),
              llvm::ConstantExpr::getBitCast(f, CtorPFTy),
              llvm::Constant::getNullValue(ltVoidPtr)};

          llvm::Constant *Ctors[] = {
                  llvm::ConstantStruct::get(CtorStructTy, S)
          };

          llvm::Function *f2 = llvm::Function::Create(ft, llvm::Function::InternalLinkage, "qD", module);
          llvm::BasicBlock *bb2 = llvm::BasicBlock::Create(ctx, "entry", f2);
          dtorBuilder.SetInsertPoint(bb2);
          dtorBuilder.SetInsertPoint(dtorBuilder.CreateRetVoid());

            llvm::Constant *Dtors[] = {
                    llvm::ConstantStruct::get(CtorStructTy,
                            llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), 65535, false),
                            llvm::ConstantExpr::getBitCast(f2, CtorPFTy),
                            llvm::Constant::getNullValue(ltVoidPtr),
                            nullptr)
            };

            llvm::ArrayType *AT = llvm::ArrayType::get(CtorStructTy, 1);

        new llvm::GlobalVariable(*module, AT, false,
                                       llvm::GlobalValue::AppendingLinkage,
                                       llvm::ConstantArray::get(AT, Ctors),
                                       "llvm.global_ctors");

        new llvm::GlobalVariable(*module, AT, false,
                                   llvm::GlobalValue::AppendingLinkage,
                                   llvm::ConstantArray::get(AT, Dtors),
                                   "llvm.global_dtors");

    }

    ~NewBackend() {
    }

    LocalVariableData createLocalVariable(const std::string &name) {
        //TODO do this in entry BB? Or make analyzer 2-pass?

//        location(range);
        llvm::AllocaInst *v = builder.CreateAlloca(ltQoreValue, nullptr, name);
        builder.CreateStore(nothing, v);
        //lifetime start
        return v;
    }

    void destroy(LocalVariableData d) {
        //load & deref
    }

    void destroy(StringLiteralData) {
    }

    void destroy(Value v) {
        call(fnStrongDeref).withQoreValueArg(v).build();
    }

    Value load(const LocalVariableData &sl) {
        Value val = builder.CreateLoad(sl);         //TODO align, volatile
        call(fnStrongRef).withQoreValueArg(val).build();
        return val;
    }

    LValue loadPtr(LocalVariableData var) {
        return var;
    }

    StringLiteralData createStringLiteral(const std::string &value /*, const SourceRange &range*/) {
//        location(range);
        llvm::GlobalVariable *gv1 = new llvm::GlobalVariable(*module, ltQoreValue, false, llvm::GlobalValue::PrivateLinkage, nothing);
        gv1->setUnnamedAddr(true);

        llvm::Constant *v = llvm::ConstantDataArray::getString(ctx, value, true);
        llvm::GlobalVariable *gv = new llvm::GlobalVariable(*module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v);
        gv->setUnnamedAddr(true);

        ctorBuilder.CreateStore(CallBuilder(ctorBuilder, fnMakeStr).withGenericArg(ctorBuilder.CreateConstGEP2_32(nullptr, gv, 0, 0)).build(), gv1);
        llvm::LoadInst *vx = dtorBuilder.CreateLoad(gv1);
        CallBuilder(dtorBuilder, fnStrongDeref).withQoreValueArg(vx).build();
        dtorBuilder.SetInsertPoint(vx);

        return gv1;
    }
//
    Value load(const StringLiteralData &sl) {
        Value val = builder.CreateLoad(sl);         //TODO align, volatile
        call(fnStrongRef).withQoreValueArg(val).build();
        return val;
    }

    void swap(LValue v1, Value &v2) {
        llvm::Value *tmp1 = builder.CreateLoad(v1);
        builder.CreateStore(v2, v1);
        v2 = tmp1;
    }

    Value loadUnique(LValue lval) {
        return call(fnLoadUnique).withQoreValuePtrArg(lval).build();
    }

    void trim(Value &v) {
        call(fnEvalTrim).withQoreValueArg(v).build();
    }

    Value add(const Value &l, const Value &r) {
        return call(fnEvalAdd).withQoreValueArg(l).withQoreValueArg(r).build();
    }

    void print(const Value &v) {
        call(fnPrintQv).withQoreValueArg(v).build();
    }

    void ret() {
        builder.CreateRetVoid();
        module->dump();
    }

    llvm::Module *getModule() {
        return module;
    }

private:
    llvm::IRBuilder<> ctorBuilder{ctx};
    llvm::IRBuilder<> dtorBuilder{ctx};
};


std::unique_ptr<llvm::Module> doCodeGen(const qore::analyzer::Script &script) {
    NewBackend cg;
    Runner<NewBackend> r(script, cg);
    r.run();
    return std::unique_ptr<llvm::Module>(cg.getModule());
}


} // namespace qore

#endif // TOOLS_DRIVER_GEN2_H_
//TODO LLVM mustn't reorder things!  -> volatile
