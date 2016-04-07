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
#ifndef LIB_CG_HELPER_H_
#define LIB_CG_HELPER_H_

#include <string>
#include <unordered_map>
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "qore/common/Exceptions.h"
#include "qore/common/Util.h"

namespace qore {
namespace cg {

class Helper {

public:
    Helper() : ctx(llvm::getGlobalContext()) {
        module = util::make_unique<llvm::Module>("Q", ctx);
        lt_void = llvm::Type::getVoidTy(ctx);
        lt_qint = llvm::Type::getInt64Ty(ctx);
        lt_qptr = llvm::StructType::create(ctx, "Q")->getPointerTo();
        lt_qsize = llvm::Type::getInt64Ty(ctx);
        lt_qvalue = llvm::StructType::create(lt_qint, "qvalue", false);
        lt_qvalue_ptr = lt_qvalue->getPointerTo();
        lt_GlobalVariable_ptr = llvm::StructType::create(ctx, "GlobalVariable")->getPointerTo();

        lf_incRef = createFunction("incRef", lt_void, lt_qvalue);
        lf_decRef = createFunction("decRef", lt_void, lt_qvalue);

        lf_gv_create = createFunction("gv_create", lt_GlobalVariable_ptr, lt_qvalue);
        lf_gv_free = createFunction("gv_free", lt_qvalue, lt_GlobalVariable_ptr);
        lf_gv_read_lock = createFunction("gv_read_lock", lt_qvalue, lt_GlobalVariable_ptr);
        lf_gv_read_unlock = createFunction("gv_read_unlock", lt_void, lt_GlobalVariable_ptr);
        lf_gv_write_lock = createFunction("gv_write_lock", lt_qvalue_ptr, lt_GlobalVariable_ptr);
        lf_gv_write_unlock = createFunction("gv_write_unlock", lt_void, lt_GlobalVariable_ptr);

        lf_qint_to_qvalue = createFunction("qint_to_qvalue", lt_qvalue, lt_qint);

        lf_createString = createFunction("createString", lt_qvalue, llvm::Type::getInt8PtrTy(ctx), lt_qsize);

        //FIXME this can be done generically using rt::meta::*Desc
        functions[&ir::Functions::IntToString] = createFunction("convertIntToString", lt_qvalue, lt_qvalue);
        functions[&ir::Functions::StringToInt] = createFunction("convertStringToInt", lt_qvalue, lt_qvalue);
        functions[&ir::Functions::StringPlusString] = createFunction("opAddStringString", lt_qvalue, lt_qvalue,
                lt_qvalue);
        functions[&ir::Functions::IntPlusInt] = createFunction("opAddIntInt", lt_qvalue, lt_qvalue, lt_qvalue);
        functions[&ir::Functions::BoxInt] = createFunction("int_box", lt_qvalue, lt_qvalue);
        functions[&ir::Functions::AnyPlusAny] = createFunction("op_add_any_any", lt_qvalue, lt_qvalue, lt_qvalue);
        functions[&ir::Functions::AnyPlusEqAny] = createFunction("op_addeq_any_any", lt_qvalue, lt_qvalue, lt_qvalue);
    }

    llvm::Function *createFunction(const std::string &name, llvm::Type *ret) {
        return llvm::Function::Create(
                llvm::FunctionType::get(ret, llvm::ArrayRef<llvm::Type *>(), false),
                llvm::Function::ExternalLinkage, name, module.get());
    }

    llvm::Function *createFunction(const std::string &name, llvm::Type *ret, llvm::Type *arg1) {
        return llvm::Function::Create(
                llvm::FunctionType::get(ret, arg1, false),
                llvm::Function::ExternalLinkage, name, module.get());
    }

    llvm::Function *createFunction(const std::string &name, llvm::Type *ret, llvm::Type *arg1, llvm::Type *arg2) {
        llvm::Type *args[2] = {arg1, arg2};
        return llvm::Function::Create(
                llvm::FunctionType::get(ret, args, false),
                llvm::Function::ExternalLinkage, name, module.get());
    }

    llvm::Value *call(llvm::IRBuilder<> builder, const ir::Function &f, llvm::ArrayRef<llvm::Value *> args) const {
        auto it = functions.find(&f);
        if (it != functions.end()) {
            return builder.CreateCall(it->second, args);
        }
        return nullptr;
    }

public:
    llvm::LLVMContext &ctx;
    std::unique_ptr<llvm::Module> module;

    llvm::Type *lt_void;
    llvm::Type *lt_qint;
    llvm::Type *lt_qptr;
    llvm::Type *lt_qsize;
    llvm::Type *lt_qvalue;
    llvm::Type *lt_qvalue_ptr;
    llvm::Type *lt_GlobalVariable_ptr;

    llvm::Function *lf_incRef;
    llvm::Function *lf_decRef;

    llvm::Function *lf_gv_create;
    llvm::Function *lf_gv_free;
    llvm::Function *lf_gv_read_lock;
    llvm::Function *lf_gv_read_unlock;
    llvm::Function *lf_gv_write_lock;
    llvm::Function *lf_gv_write_unlock;

    llvm::Function *lf_qint_to_qvalue;

    llvm::Function *lf_createString;

private:
    std::unordered_map<const ir::Function *, llvm::Function *> functions;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_HELPER_H_
