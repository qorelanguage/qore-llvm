#ifndef TOOLS_DRIVER_GEN_H_
#define TOOLS_DRIVER_GEN_H_

#include "ast.h"

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
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {llvm::Type::getInt32Ty(ctx)}, false);
        printFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "print", nullptr);
        ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), {llvm::Type::getInt8PtrTy(ctx)}, false);
        printStrFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "print_str", nullptr);
        module->getFunctionList().push_back(printFunction);
        module->getFunctionList().push_back(printStrFunction);
    }

    R visit(const class IntegerLiteral *e) override {
        return llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx), e->getValue(), true);
    }

    R visit(const class StringLiteral *e) override {
        llvm::Constant *v = llvm::ConstantDataArray::getString(ctx, e->getValue(), true);
        llvm::GlobalVariable *gv = new llvm::GlobalVariable(*module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v);
        gv->setUnnamedAddr(true);
        return builder.CreateConstGEP2_32(gv, 0, 0);
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
        if (value->getType()->isPointerTy()) {
            builder.CreateCall(printStrFunction, value);
        } else {
            builder.CreateCall(printFunction, value);
        }
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

    llvm::Function *printFunction;
    llvm::Function *printStrFunction;
};

#endif /* TOOLS_DRIVER_GEN_H_ */
