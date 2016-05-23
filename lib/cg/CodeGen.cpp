//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 Qore Technologies
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
///
/// \file
/// \brief Implements the code generator.
///
//------------------------------------------------------------------------------
#include "qore/cg/CodeGen.h"
#include <iostream>
#include <string>
#include <vector>
#include "llvm/IR/AssemblyAnnotationWriter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"
#include "FunctionCompiler.h"

namespace qore {
namespace cg {

///\cond
class Compiler {

public:
    Compiler() : qstart(helper.createFunction("qstart", helper.lt_void, helper.lt_Env_ptr)), builder(helper.ctx) {
        builder.SetInsertPoint(llvm::BasicBlock::Create(helper.ctx, "entry", qstart));
    }

    std::unique_ptr<llvm::Module> compile(Env &env) {
        llvm::Value *rtEnv = &*qstart->arg_begin();
        llvm::Value *rtNs = builder.CreateCall(helper.lf_env_getRootNamespace, rtEnv);

        for (const SourceInfo &src : env.getSourceInfos()) {
            llvm::Value *args[2] = { rtEnv, stringLiteral(src.getName(), "src_info") };
            sourceInfos[&src] = builder.CreateCall(helper.lf_env_addSourceInfo, args);
        }

        for (const String &str : env.getStrings()) {
            llvm::GlobalVariable *strGv = new llvm::GlobalVariable(*helper.module, helper.lt_qvalue, false,
                    llvm::GlobalVariable::PrivateLinkage, llvm::Constant::getNullValue(helper.lt_qvalue), "str");


            llvm::Value *args[2] = { rtEnv, stringLiteral(str.get(), "str_lit") };
            llvm::Value *v = builder.CreateCall(helper.lf_env_addString, args);
            builder.CreateStore(v, strGv);
            strings[&str] = strGv;
        }

        compile(rtNs, env.getRootNamespace());
        builder.CreateRetVoid();

        for (auto &p : functions) {
            FunctionCompiler fc(*p.first, strings, globals, functions, p.second, helper);
            fc.compile();
        }

        return std::move(helper.module);
    }

private:
    #define LOCATION(x) sourceInfos[&x.getLocation().getSourceInfo()], \
        llvm::ConstantInt::get(helper.lt_int32, x.getLocation().getPacked())
    void compile(llvm::Value *rtNs, const Namespace &ns) {
        for (auto &n : ns.getNamespaces()) {
            llvm::Value *args[4] = { rtNs, name(n.getFullName()), LOCATION(n) };
            llvm::Value *rtNs2 = builder.CreateCall(helper.lf_namespace_addNamespace, args);
            compile(rtNs2, n);
        }
        for (auto &gv : ns.getGlobalVariables()) {
            llvm::Value *args[5] = { rtNs, name(gv.getFullName()), type(gv.getType()), LOCATION(gv) };
            llvm::Value *rtGv = builder.CreateCall(helper.lf_namespace_addGlobalVariable, args);
            llvm::GlobalVariable *g = new llvm::GlobalVariable(*helper.module, helper.lt_GlobalVariable_ptr, false,
                    llvm::GlobalValue::PrivateLinkage, llvm::Constant::getNullValue(helper.lt_GlobalVariable_ptr),
                    gv.getFullName());  //FIXME mangled name
            builder.CreateStore(rtGv, g);
            globals[&gv] = g;
        }
        for (auto &fg : ns.getFunctionGroups()) {
            llvm::Value *args[2] = { rtNs, name(fg.getFullName()) };
            llvm::Value *rtFg = builder.CreateCall(helper.lf_namespace_addFunctionGroup, args);
            declare(rtFg, fg);
        }
    }
    #undef LOCATION

    void declare(llvm::Value *rtFg, const FunctionGroup &fg) {
        for (auto &f : fg.getFunctions()) {
            declare(fg.getFullName(), f);   //TODO mangled name
            //generate call for lf_functionGroup_addFunction, save the pointer to the function
        }
    }

    void declare(const std::string &name, const Function &f) {
        std::vector<llvm::Type *> args(f.getType().getParameterCount(), helper.lt_qvalue);
        llvm::Type *ret = f.getType().getReturnType() == Type::Nothing ? helper.lt_void : helper.lt_qvalue;
        llvm::Function *func = llvm::Function::Create(
                llvm::FunctionType::get(ret, args, false),
                llvm::Function::ExternalLinkage, name, helper.module.get());
        functions[&f] = func;
        Index i = 0;
        for (auto it = func->arg_begin(); it != func->arg_end(); ++it) {
            (*it).setName(llvm::Twine("arg").concat(llvm::Twine(i++)));
        }
    }

    llvm::Value *stringLiteral(const std::string &str, const std::string &name) {
        llvm::Constant *val = llvm::ConstantDataArray::getString(helper.ctx, str, true);
        llvm::GlobalVariable *gv = new llvm::GlobalVariable(*helper.module, val->getType(), true,
                llvm::GlobalValue::PrivateLinkage, val, name);
        gv->setUnnamedAddr(true);
        return builder.CreateConstGEP2_32(nullptr, gv, 0, 0);
    }

    llvm::Value *name(const std::string &str) {
        return stringLiteral(str.substr(str.rfind(':') + 1), "name");
    }

    llvm::Value *type(const Type &type) {
        llvm::Value *&ref = types[&type];
        if (!ref) {
            //must be built-in, class types will be handled in a different way
            if (type == Type::String) {
                ref = builder.CreateCall(helper.lf_type_String);
            } else {
                QORE_NOT_IMPLEMENTED("");
            }
        }
        return ref;
    }

private:
    Helper helper;
    std::unordered_map<const SourceInfo *, llvm::Value *> sourceInfos;
    FunctionContext::StringsMap strings;
    FunctionContext::GlobalsMap globals;
    FunctionContext::FunctionsMap functions;
    llvm::Function *qstart;
    llvm::IRBuilder<> builder;
    std::unordered_map<const Type *, llvm::Value *> types;
};

void CodeGen::process(Env &env) {
    Compiler compiler;
    std::unique_ptr<llvm::Module> module = compiler.compile(env);

    module->dump();

    std::cerr.flush();
    llvm::raw_os_ostream sss(std::cerr);
    if (llvm::verifyModule(*module, &sss)) {
        sss.flush();
        assert(false);
    }

    std::error_code ec;
    llvm::raw_fd_ostream os2("out.ll", ec, llvm::sys::fs::F_None);
    std::unique_ptr<llvm::AssemblyAnnotationWriter> Annotator;
    assert(!ec);
    module->print(os2, Annotator.get());
}
///\endcond

} // namespace cg
} // namespace qore
