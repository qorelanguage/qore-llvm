#ifndef TOOLS_DRIVER_GEN_H_
#define TOOLS_DRIVER_GEN_H_
#if 0
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

/**
 * \private
 */
class CodeGen {

public:
    CodeGen(qore::SourceManager &sourceMgr) : sourceMgr(sourceMgr) {
        ltVoid = llvm::Type::getVoidTy(ctx);
        ltInt64 = llvm::Type::getInt64Ty(ctx);
        ltCharPtr = llvm::Type::getInt8PtrTy(ctx);
        ltQoreValue = llvm::StructType::get(ltInt64, ltInt64, nullptr);
        ltQoreValuePtr = ltQoreValue->getPointerTo();

        fnPrintQv = f(ltVoid, "print_qv", ltQoreValuePtr, nullptr);
        fnMakeInt = f(ltVoid, "make_int", ltQoreValuePtr, ltInt64, nullptr);
        fnMakeStr = f(ltVoid, "make_str", ltQoreValuePtr, ltCharPtr, nullptr);
        fnMakeNothing = f(ltVoid, "make_nothing", ltQoreValuePtr, nullptr);
        fnEvalAdd = f(ltVoid, "eval_add", ltQoreValuePtr, ltQoreValuePtr, ltQoreValuePtr, nullptr);
        fnEvalTrim = f(ltVoid, "eval_trim", ltQoreValuePtr, nullptr);
        fnEvalAssign = f(ltVoid, "eval_assign", ltQoreValuePtr, ltQoreValuePtr, nullptr);
        fnDeref = f(ltVoid, "deref", ltQoreValuePtr, nullptr);
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

    void visit(const std::unique_ptr<qore::il::Function> &fff) {
        llvm::FunctionType *ft = llvm::FunctionType::get(llvm::Type::getVoidTy(ctx), false);
        llvm::Function *f = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "q", module);
        llvm::BasicBlock *bb = llvm::BasicBlock::Create(ctx, "entry", f);
        builder.SetInsertPoint(bb);

        //        !llvm.dbg.cu = !{!0}
        //        !0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "qore-llvm 1.0.0", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
        //        !1 = !DIFile(filename: "/home/otethal/prj/qore-llvm/tools/sandbox/test.q", directory: "/home/otethal/prj/qore-llvm/tools/sandbox")
        //        !2 = !{}
        char *c_path = strdup("/todo/path/to/source");
//        char *c_path = realpath(sourceMgr.getName(p->getRange().start.sourceId).c_str(), nullptr);
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

        diFile = diCU->getFile();

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

        //Standard types
        auto ditUint64 = dib.createBasicType("uint64_t", 64, 64, llvm::dwarf::DW_ATE_unsigned);
        auto ditInt64 = dib.createBasicType("int64_t", 64, 64, llvm::dwarf::DW_ATE_signed);
        auto ditChar = dib.createBasicType("char", 8, 8, llvm::dwarf::DW_ATE_signed_char);
        auto ditConstChar = dib.createQualifiedType(llvm::dwarf::DW_TAG_const_type, ditChar);
        auto ditConstCharPtr = dib.createPointerType(ditConstChar, 64, 64); //name

        //QoreValue Tag
        llvm::Metadata * elements[]{
                    dib.createEnumerator("Nothing", 0),
                    dib.createEnumerator("Int", 1),
                    dib.createEnumerator("Str", 2)
                };
        auto ditQoreValueTag = dib.createEnumerationType(
                diFile,             // Scope
                "Tag",
                diFile,
                1000,               // LineNumber
                64,                 // SizeInBits
                64,                 // AlignInBits
                dib.getOrCreateArray(elements),
                ditUint64, "QORE_RUNTIME_TAG"
        );

        //QoreValue
        ditQoreValue = dib.createStructType(
                diFile,             // Scope
                "QoreValue",
                diFile,
                1000,               // LineNumber
                128,                // SizeInBits
                64,                 // AlignInBits
                0,                  // Flags
                nullptr,            // DerivedFrom
                llvm::DINodeArray(),
                0,                  // RuntimeLang
                nullptr,            // VTableHolder
                "QORE_RUNTIME_QORE_VALUE"
        );

        //QoreValue union
        auto ditQoreValueUnion = dib.createUnionType(
                ditQoreValue,       // Scope
                "QoreValueUnion",
                diFile,
                1000,               // LineNumber
                64,                 // SizeInBits
                64,                 // AlignInBits
                0,                  // Flags
                llvm::DINodeArray(),
                0,                  // RuntimeLang
                "QORE_RUNTIME_QORE_VALUE_UNION"
        );

        llvm::Metadata * unionElements[]{
                dib.createMemberType(
                        ditQoreValueUnion,          //DIScope *Scope,
                        "intValue",                 //StringRef Name,
                        diFile,                     //DIFile *File,
                        1000,                       //unsigned LineNo,
                        64,                         //uint64_t SizeInBits,
                        64,                         //uint64_t AlignInBits,
                        0,                          //uint64_t OffsetInBits,
                        llvm::DINode::FlagPublic,   //unsigned Flags,
                        ditInt64
                ),
                dib.createMemberType(
                        ditQoreValueUnion,          //DIScope *Scope,
                        "strValue",                 //StringRef Name,
                        diFile,                     //DIFile *File,
                        1000,                       //unsigned LineNo,
                        64,                         //uint64_t SizeInBits,
                        64,                         //uint64_t AlignInBits,
                        0,                          //uint64_t OffsetInBits,
                        llvm::DINode::FlagPublic,   //unsigned Flags,
                        ditConstCharPtr
                )
        };
        dib.replaceArrays(ditQoreValueUnion, dib.getOrCreateArray(unionElements));

        llvm::Metadata * structElements[]{
                dib.createMemberType(
                        ditQoreValue,               //DIScope *Scope,
                        "tag",                      //StringRef Name,
                        diFile,                     //DIFile *File,
                        1000,                       //unsigned LineNo,
                        64,                         //uint64_t SizeInBits,
                        64,                         //uint64_t AlignInBits,
                        0,                          //uint64_t OffsetInBits,
                        llvm::DINode::FlagPublic,   //unsigned Flags,
                        ditQoreValueTag
                ),
                dib.createMemberType(
                        ditQoreValue,               //DIScope *Scope,
                        "value",                    //StringRef Name,
                        diFile,                     //DIFile *File,
                        1000,                       //unsigned LineNo,
                        64,                         //uint64_t SizeInBits,
                        64,                         //uint64_t AlignInBits,
                        64,                         //uint64_t OffsetInBits,
                        llvm::DINode::FlagPublic,   //unsigned Flags,
                        ditQoreValueUnion
                )
        };
        dib.replaceArrays(ditQoreValue, dib.getOrCreateArray(structElements));

        for (auto &c : fff->constants) {
            llvm::Constant *v = llvm::ConstantDataArray::getString(ctx, c->getValue(), true);
            llvm::GlobalVariable *gv = new llvm::GlobalVariable(*module, v->getType(), true, llvm::GlobalValue::PrivateLinkage, v, "strLiteralValue");
            gv->setUnnamedAddr(true);

//                        location(e);
            llvm::Value *value = builder.CreateAlloca(ltQoreValue, nullptr, "strLiteral");
            llvm::Value* args[] = {value, builder.CreateConstGEP2_32(nullptr, gv, 0, 0)};
            builder.CreateCall(fnMakeStr, args);
            c->tag = value;
        }



        //Compile code
        for (const auto &a : fff->actions) {
            switch (a.type) {
                case qore::il::Action::Add: {
//                    location(e->operatorRange);
                    llvm::Value* args[] = {V(a.s1), V(a.s2), V(a.s3)};
                    builder.CreateCall(fnEvalAdd, args);
                    break;
                }
                case qore::il::Action::Assign: {
//                    location(e->operatorRange);
                    llvm::Value* args[] = {V(a.s1), V(a.s2)};
                    builder.CreateCall(fnEvalAssign, args);
                    break;
                }
                case qore::il::Action::LifetimeStart: {
//                    location(e);
                    auto *v = builder.CreateAlloca(ltQoreValue, nullptr, "var");

//                        llvm::DIExpression *diExpr = dib.createExpression();
//                        llvm::DILocalVariable *diLocVar = dib.createLocalVariable(
//                                llvm::dwarf::DW_TAG_auto_variable,
//                                scope,
//                                e->name,
//                                diFile,
//                                e->getRange().start.line,
//                                ditQoreValue
//                        );
//
//                        dib.insertDeclare(v, diLocVar, diExpr,
//                                llvm::DILocation::get(ctx, e->getRange().start.line, e->getRange().start.column, scope),
//                                builder.GetInsertBlock());


                    llvm::Value* args[] = {v};
                    builder.CreateCall(fnMakeNothing, args);
                    a.s1->tag = v;
                    break;
                }
                case qore::il::Action::LifetimeEnd: {
                    llvm::Value* args[] = {V(a.s1)};
                    builder.CreateCall(fnDeref, args);
                    break;
                }
                case qore::il::Action::Print: {
//                    location(s);
                    llvm::Value* args[] = {V(a.s1)};
                    builder.CreateCall(fnPrintQv, args);
                    break;
                }
                case qore::il::Action::Return:
                    //builder.SetCurrentDebugLocation(llvm::DILocation::get(ctx, p->getRange().end.line, p->getRange().end.column, scope));

                    for (auto &c : fff->constants) {
                        llvm::Value* args[] = {V(c.get())};
                        builder.CreateCall(fnDeref, args);
                    }

                    builder.CreateRetVoid();
                    break;
                case qore::il::Action::Trim: {
//                    location(e->operatorRange);
                    llvm::Value* args[] = {V(a.s1)};
                    builder.CreateCall(fnEvalTrim, args);
                    break;
                }
                default:
                    QORE_UNREACHABLE("NOT IMPLEMENTED: " << a);
            }
        }

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
    static inline llvm::Value *V(const qore::il::AValue *s) {
        return static_cast<llvm::Value*>(s->tag);
    }

private:
    llvm::LLVMContext &ctx{llvm::getGlobalContext()};
    llvm::Module *module{new llvm::Module("Q", ctx)};
    llvm::DIBuilder dib{*module};
    llvm::IRBuilder<> builder{ctx};

    llvm::Type *ltVoid;
    llvm::Type *ltInt64;
    llvm::Type *ltCharPtr;
    llvm::StructType *ltQoreValue;
    llvm::Type *ltQoreValuePtr;
    llvm::Function *fnPrintQv;
    llvm::Function *fnMakeInt;
    llvm::Function *fnMakeStr;
    llvm::Function *fnMakeNothing;
    llvm::Function *fnEvalAdd;
    llvm::Function *fnEvalTrim;
    llvm::Function *fnEvalAssign;
    llvm::Function *fnDeref;

    llvm::DISubprogram *scope;

    qore::SourceManager &sourceMgr;

    llvm::DIFile *diFile;
    llvm::DICompositeType *ditQoreValue;
};

std::unique_ptr<llvm::Module> doCodeGen(qore::SourceManager &sourceMgr, const std::unique_ptr<qore::il::Function> &f) {
    CodeGen cg(sourceMgr);
    cg.visit(f);
    return std::unique_ptr<llvm::Module>(cg.getModule());
}
#endif
#endif /* TOOLS_DRIVER_GEN_H_ */
