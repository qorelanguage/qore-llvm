#ifndef TOOLS_DRIVER_GEN_H_
#define TOOLS_DRIVER_GEN_H_

#include <map>
#include <string>
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
#include "llvm/Support/raw_os_ostream.h"

/**
 * \private
 */
class CodeGenVisitor : public qore::ast::Visitor {

public:
    CodeGenVisitor(qore::SourceManager &sourceMgr) : sourceMgr(sourceMgr) {
        ltVoid = llvm::Type::getVoidTy(ctx);
        ltInt64 = llvm::Type::getInt64Ty(ctx);
        ltCharPtr = llvm::Type::getInt8PtrTy(ctx);
        ltQoreValue = llvm::StructType::get(ltInt64, ltInt64, nullptr);
        ltQoreValuePtr = ltQoreValue->getPointerTo();

        fnPrintQv = f(ltVoid, "print_qv", ltQoreValuePtr, nullptr);
        fnMakeNothing = f(ltVoid, "make_nothing", ltQoreValuePtr, nullptr);
        fnMakeInt = f(ltVoid, "make_int", ltQoreValuePtr, ltInt64, nullptr);
        fnMakeStr = f(ltVoid, "make_str", ltQoreValuePtr, ltCharPtr, nullptr);
        fnEvalAdd = f(ltVoid, "eval_add", ltQoreValuePtr, ltQoreValuePtr, ltQoreValuePtr, nullptr);
        fnEvalTrim = f(ltVoid, "eval_trim", ltQoreValuePtr, nullptr);
        fnEvalAssign = f(ltVoid, "eval_assign", ltQoreValuePtr, ltQoreValuePtr, nullptr);
        scope = nullptr;
    }

    llvm::Module *getModule() {
        return module;
    }
//    // this creates the int constant in the rodata section
//    R visit(const qore::ast::IntegerLiteral *e) override {
//        builder.SetCurrentDebugLocation(llvm::DILocation::get(ctx, e->getRange().start.line, e->getRange().start.column, scope));
//        llvm::Constant *v = llvm::ConstantStruct::get(ltQoreValue, llvm::ConstantInt::get(ltInt64, 1), llvm::ConstantInt::get(ltInt64, e->value), nullptr);
//        llvm::GlobalVariable *gv = new llvm::GlobalVariable(*module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v, "intLiteral");
//        gv->setUnnamedAddr(true);
//        currentValue = gv;
//        return nullptr;
//    }

    void location(const qore::SourceLocation &l) {
        builder.SetCurrentDebugLocation(llvm::DILocation::get(ctx, l.line, l.column, scope));
    }

    void location(const qore::SourceRange &r) {
        location(r.start);
    }

    void location(const qore::ast::Node *node) {
        location(node->getRange());
    }

    void visit(const qore::ast::IntegerLiteral *e) override {
        location(e);
        currentValue = builder.CreateAlloca(ltQoreValue, nullptr, "intLiteral");        //TODO these are not reused
        llvm::Value* args[] = {currentValue, llvm::ConstantInt::get(ltInt64, e->value, true)};
        builder.CreateCall(fnMakeInt, args);
    }

    void visit(const qore::ast::StringLiteral *e) override {
        llvm::Constant *v = llvm::ConstantDataArray::getString(ctx, e->value, true);
        llvm::GlobalVariable *gv = new llvm::GlobalVariable(*module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v, "strLiteralValue");
        gv->setUnnamedAddr(true);

        location(e);
        currentValue = builder.CreateAlloca(ltQoreValue, nullptr, "strLiteral");
        llvm::Value* args[] = {currentValue, builder.CreateConstGEP2_32(nullptr, gv, 0, 0)};
        builder.CreateCall(fnMakeStr, args);
    }

    void visit(const qore::ast::BinaryExpression *e) override {
        e->left->accept(*this);
        llvm::Value *l = currentValue;
        e->right->accept(*this);
        llvm::Value *r = currentValue;

        location(e->operatorRange);
        currentValue = builder.CreateAlloca(ltQoreValue, nullptr, "sum");
        llvm::Value* args[] = {currentValue, l, r};
        builder.CreateCall(fnEvalAdd, args);
    }

    void visit(const qore::ast::UnaryExpression *e) override {
        e->operand->accept(*this);
        location(e->operatorRange);
        llvm::Value* args[] = {currentValue};
        builder.CreateCall(fnEvalTrim, args);
    }

    void visit(const qore::ast::Assignment *e) override {
        e->left->accept(*this);
        llvm::Value *l = currentValue;
        e->right->accept(*this);
        llvm::Value *r = currentValue;

        location(e->operatorRange);
        llvm::Value* args[] = {l, r};
        builder.CreateCall(fnEvalAssign, args);
    }

    void visit(const qore::ast::VarDecl *e) override {
        location(e);
        currentValue = builder.CreateAlloca(ltQoreValue, nullptr, "var_" + e->name);
        llvm::Value* args[] = {currentValue};
        builder.CreateCall(fnMakeNothing, args);
        variables[e->name] = currentValue;
    }

    void visit(const qore::ast::Identifier *e) override {
        currentValue = variables[e->name];
    }

    void visit(const qore::ast::EmptyStatement *) override {
    }

    void visit(const qore::ast::PrintStatement *s) override {
        s->expression->accept(*this);
        location(s);
        llvm::Value* args[] = {currentValue};
        builder.CreateCall(fnPrintQv, args);
    }

    void visit(const qore::ast::ExpressionStatement *s) override {
        s->expression->accept(*this);
    }

    void visit(const qore::ast::Program *p) override {
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "q", module);
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "entry", f);
        builder.SetInsertPoint(bb);

        llvm::DIBuilder dib(*module);

        //        !llvm.dbg.cu = !{!0}
        //        !0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "qore-llvm 1.0.0", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
        //        !1 = !DIFile(filename: "/home/otethal/prj/qore-llvm/tools/sandbox/test.q", directory: "/home/otethal/prj/qore-llvm/tools/sandbox")
        //        !2 = !{}
        char *c_path = realpath(sourceMgr.getName(p->getRange().start.sourceId).c_str(), nullptr);
        std::string fileName(c_path);
        free(c_path);   //TODO exception safety
        std::string path(fileName);
        auto i = path.rfind('/');
        if (i != std::string::npos) {
            path.resize(i);
        }
        llvm::DICompileUnit *diCU = dib.createCompileUnit(llvm::dwarf::DW_LANG_C,
                fileName, path,
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

        builder.SetCurrentDebugLocation(llvm::DILocation::get(ctx, p->getRange().end.line, p->getRange().end.column, scope));
        builder.CreateRetVoid();
        llvm::verifyFunction(*f);

        dib.finalize();

        module->dump();
        llvm::raw_os_ostream sss(std::cout);
        llvm::verifyModule(*module, &sss);
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

private:
    llvm::LLVMContext &ctx{llvm::getGlobalContext()};
    llvm::Module *module{new llvm::Module("Q", ctx)};
    llvm::IRBuilder<> builder{ctx};
    std::map<std::string, llvm::Value*> variables;

    llvm::Value *currentValue;

    llvm::Type *ltVoid;
    llvm::Type *ltInt64;
    llvm::Type *ltCharPtr;
    llvm::StructType *ltQoreValue;
    llvm::Type *ltQoreValuePtr;
    llvm::Function *fnPrintQv;
    llvm::Function *fnMakeNothing;
    llvm::Function *fnMakeInt;
    llvm::Function *fnMakeStr;
    llvm::Function *fnEvalAdd;
    llvm::Function *fnEvalTrim;
    llvm::Function *fnEvalAssign;

    llvm::DISubprogram *scope;

    qore::SourceManager &sourceMgr;
};

#endif /* TOOLS_DRIVER_GEN_H_ */
