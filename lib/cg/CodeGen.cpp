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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#include "qore/cg/CodeGen.h"
#include <iostream>
#include <vector>
#include "llvm/IR/AssemblyAnnotationWriter.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_os_ostream.h"

#include "FunctionCompiler.h"

namespace qore {
namespace cg {

void CodeGen::process(comp::as::Script &script) {
    Helper helper;

    llvm::GlobalVariable *rtctx = new llvm::GlobalVariable(*helper.module, helper.lt_Context, false,
            llvm::GlobalValue::ExternalLinkage, nullptr, "rtctx");
    std::unordered_map<comp::as::Function *, llvm::Function *> functions;

    for (auto &f : script.getFunctions()) {
        std::vector<llvm::Type *> args(f->getArgCount(), helper.lt_qvalue);
        llvm::Type *ret = f->isVoid() ? helper.lt_void : helper.lt_qvalue;
        llvm::Function *func = llvm::Function::Create(
                llvm::FunctionType::get(ret, args, false),
                llvm::Function::ExternalLinkage, f->getName(), helper.module.get());
        functions[f.get()] = func;
        Id i = 0;
        for (auto it = func->arg_begin(); it != func->arg_end(); ++it) {
            (*it).setName(llvm::Twine("arg").concat(llvm::Twine(i)));
        }
    }

    for (auto &f : script.getFunctions()) {
        FunctionCompiler fc(*f, functions[f.get()], helper, rtctx);
        fc.x();
    }

    std::unique_ptr<llvm::Module> module = std::move(helper.module);

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

} // namespace cg
} // namespace qore
