#ifndef TOOLS_DRIVER_GEN_H_
#define TOOLS_DRIVER_GEN_H_

#include "qore/ast/ast.h"

#include "llvm/ADT/APSInt.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

class CodeGenVisitor : public Visitor {

public:
    CodeGenVisitor() {
        qvStruct = llvm::StructType::create(ctx, "QoreValue");
        qvStruct->setBody(llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx), nullptr);
        llvm::Type* a[] = {llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx)};
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), a, false);
        printQvFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "print_qv", nullptr);
        ft = llvm::FunctionType::get(qvStruct, {llvm::Type::getInt64Ty(ctx)}, false);
        makeIntFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "make_int", nullptr);
        ft = llvm::FunctionType::get(qvStruct, {llvm::Type::getInt8PtrTy(ctx)}, false);
        makeStrFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "make_str", nullptr);
//TODO declare these exactly as clang does, otherwise LTO does not work
        module->getFunctionList().push_back(printQvFunction);
        module->getFunctionList().push_back(makeIntFunction);
        module->getFunctionList().push_back(makeStrFunction);
    }

    R visit(const class IntegerLiteral *e) override {
        auto c = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), e->getValue(), true);
        return builder.CreateCall(makeIntFunction, c);
    }

    R visit(const class StringLiteral *e) override {
        llvm::Constant *v = llvm::ConstantDataArray::getString(ctx, e->getValue(), true);
        llvm::GlobalVariable *gv = new llvm::GlobalVariable(*module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v);
        gv->setUnnamedAddr(true);
        auto c = builder.CreateConstGEP2_32(gv, 0, 0);
        return builder.CreateCall(makeStrFunction, c);
    }

    R visit(const class VariableLoadExpression *e) override {
        return builder.CreateLoad(variables[e->getName()], e->getName());
    }

    R visit(const class Assignment *e) override {
        llvm::Value *val = static_cast<llvm::Value*>(e->getValue()->accept(*this));
        llvm::AllocaInst *ai = builder.CreateAlloca(llvm::Type::getInt32Ty(ctx), nullptr, e->getVarName());
        builder.CreateStore(val, ai);
        variables[e->getVarName()] = ai;
        return val;
    }

    R visit(const class ExpressionStatement *s) override {
        return s->getExpression()->accept(*this);
    }

    R visit(const class PrintStatement *s) override {
        llvm::Value *value = static_cast<llvm::Value*>(s->getExpression()->accept(*this));
        auto tag = builder.CreateExtractValue(value, {0}, "qv.tag");
        auto val = builder.CreateExtractValue(value, {1}, "qv.val");
        llvm::Value* args[] = {tag, val};
        builder.CreateCall(printQvFunction, args);
        return nullptr;
    }

    R visit(const class Program *p) override {
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "q", module);
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "entry", f);
        builder.SetInsertPoint(bb);
        p->forEachStatement([this](const Statement *s){s->accept(*this);});
        builder.CreateRetVoid();
        llvm::verifyFunction(*f);
        return module;
    }

private:
    llvm::LLVMContext &ctx{llvm::getGlobalContext()};
    llvm::Module *module{new llvm::Module("Q", ctx)};
    llvm::IRBuilder<> builder{ctx};
    std::map<std::string, llvm::AllocaInst*> variables;

    llvm::StructType *qvStruct;
    llvm::Function *printQvFunction;
    llvm::Function *makeIntFunction;
    llvm::Function *makeStrFunction;
};

#endif /* TOOLS_DRIVER_GEN_H_ */
