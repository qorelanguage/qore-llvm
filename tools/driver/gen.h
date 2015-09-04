#ifndef TOOLS_DRIVER_GEN_H_
#define TOOLS_DRIVER_GEN_H_

#include "qore/ast/Program.h"

#include "llvm/ADT/APSInt.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/DIBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

/**
 * \private
 */
class CodeGenVisitor : public qore::ast::Visitor {

public:
    using R = void*;

    CodeGenVisitor() {
        llvm::Type* a[] = {llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx)};
        qvStruct = llvm::StructType::get(ctx, a, false);
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), a, false);
        printQvFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "print_qv", nullptr);
        ft = llvm::FunctionType::get(qvStruct, {llvm::Type::getInt64Ty(ctx)}, false);
        makeIntFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "make_int", nullptr);
        ft = llvm::FunctionType::get(qvStruct, {llvm::Type::getInt8PtrTy(ctx)}, false);
        makeStrFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "make_str", nullptr);
        llvm::Type* qv2[] = {llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx), llvm::Type::getInt64Ty(ctx)};
        ft = llvm::FunctionType::get(qvStruct, qv2, false);
        evalAddFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "eval_add", nullptr);

        module->getFunctionList().push_back(printQvFunction);
        module->getFunctionList().push_back(makeIntFunction);
        module->getFunctionList().push_back(makeStrFunction);
        module->getFunctionList().push_back(evalAddFunction);
        scope = nullptr;
    }

    R visit(const qore::ast::IntegerLiteral *e) override {
        auto c = llvm::ConstantInt::get(llvm::Type::getInt64Ty(ctx), e->value, true);
        return builder.CreateCall(makeIntFunction, c);
    }

    R visit(const qore::ast::StringLiteral *e) override {
        llvm::Constant *v = llvm::ConstantDataArray::getString(ctx, e->value, true);
        llvm::GlobalVariable *gv = new llvm::GlobalVariable(*module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v);
        gv->setUnnamedAddr(true);
        auto c = builder.CreateConstGEP2_32(nullptr, gv, 0, 0);
        return builder.CreateCall(makeStrFunction, c);
    }

//    R visit(const qore::ast::VariableLoadExpression *e) override {
//        return builder.CreateLoad(variables[e->getName()], e->getName());
//    }

//    R visit(const qore::ast::Assignment *e) override {
//        llvm::Value *val = static_cast<llvm::Value*>(e->getValue()->accept(*this));
//        llvm::AllocaInst *ai = builder.CreateAlloca(llvm::Type::getInt32Ty(ctx), nullptr, e->getVarName());
//        builder.CreateStore(val, ai);
//        variables[e->getVarName()] = ai;
//        return val;
//    }

    R visit(const qore::ast::BinaryExpression *e) override {
        llvm::Value *l = static_cast<llvm::Value*>(e->left->accept(*this));
        llvm::Value *r = static_cast<llvm::Value*>(e->right->accept(*this));
        auto tagL = builder.CreateExtractValue(l, {0}, "l.tag");
        auto valL = builder.CreateExtractValue(l, {1}, "l.val");
        auto tagR = builder.CreateExtractValue(r, {0}, "r.tag");
        auto valR = builder.CreateExtractValue(r, {1}, "r.val");
        llvm::Value* args[] = {tagL, valL, tagR, valR};
        return builder.CreateCall(evalAddFunction, args);
    }

    R visit(const qore::ast::EmptyStatement *) override {
        return nullptr;
    }

//    R visit(const qore::ast::ExpressionStatement *s) override {
//        return s->getExpression()->accept(*this);
//    }

    R visit(const qore::ast::PrintStatement *s) override {
        llvm::Value *value = static_cast<llvm::Value*>(s->expression->accept(*this));
        auto tag = builder.CreateExtractValue(value, {0}, "qv.tag");
        auto val = builder.CreateExtractValue(value, {1}, "qv.val");
        llvm::Value* args[] = {tag, val};
        builder.SetCurrentDebugLocation(llvm::DILocation::get(ctx, currentLine++, 2, scope));
        builder.CreateCall(printQvFunction, args);
        builder.SetCurrentDebugLocation(nullptr);
        return nullptr;
    }

    R visit(const qore::ast::Program *p) override {
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "q", module);
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "entry", f);
        builder.SetInsertPoint(bb);

        llvm::DIBuilder dib(*module);

        //        !llvm.dbg.cu = !{!0}
        //        !0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "qore-llvm 1.0.0", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
        //        !1 = !DIFile(filename: "/home/otethal/prj/qore-llvm/tools/sandbox/test.q", directory: "/home/otethal/prj/qore-llvm/tools/sandbox")
        //        !2 = !{}
        llvm::DICompileUnit *diCU = dib.createCompileUnit(llvm::dwarf::DW_LANG_C,
                "/home/otethal/prj/qore-llvm/tools/sandbox/test.q", "/home/otethal/prj/qore-llvm/tools/sandbox",
                "qore-llvm 1.0.0",
                false,  //isOpt
                "",     //flags
                0);     //runtime version

        llvm::DIFile *diFile = diCU->getFile();

        //        !5 = !DISubroutineType(types: !6)
        //        !6 = !{null}
        llvm::DISubroutineType *diSubType = dib.createSubroutineType(diFile, dib.getOrCreateTypeArray({nullptr}));

        //        !3 = !{!4}
        //        !4 = !DISubprogram(name: "q", scope: !1, file: !1, line: 1, type: !5, isLocal: false, isDefinition: true, scopeLine: 1, isOptimized: false, function: void ()* @q, variables: !2)
        //TODO flags: DIFlagPrototyped ?
        llvm::DISubprogram *diSub = dib.createFunction(diFile, "q", "", diFile, 1, diSubType, false, true, 1);
        diSub->replaceFunction(f);

        scope = diSub;

        //        llvm.module.flags = !{!7, !8}
        //        !7 = !{i32 2, !"Dwarf Version", i32 4}
        //        !8 = !{i32 2, !"Debug Info Version", i32 3}
        module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", llvm::dwarf::DWARF_VERSION);
        module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", llvm::DEBUG_METADATA_VERSION);

        //        !llvm.ident = !{!9}
        //        !9 = !{!"qore-llvm 1.0.0"}
        llvm::Metadata *ident = llvm::MDString::get(ctx, "qore-llvm 1.0.0");
        module->getOrInsertNamedMetadata("llvm.ident")->addOperand(llvm::MDNode::get(ctx, {ident}));

        for (const auto &statement : p->body) {
            statement->accept(*this);
        }
        builder.CreateRetVoid();
        llvm::verifyFunction(*f);

        dib.finalize();

        module->dump();
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
    llvm::Function *evalAddFunction;

    llvm::DISubprogram *scope;
    int currentLine{1};
};

#endif /* TOOLS_DRIVER_GEN_H_ */
