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

        lt_char_ptr = llvm::Type::getInt8PtrTy(ctx);
        lt_int32 = llvm::Type::getInt32Ty(ctx);
        lt_exc = llvm::StructType::get(lt_char_ptr, lt_int32, nullptr);

        lf_personality = llvm::Function::Create(llvm::FunctionType::get(lt_int32, true),
                llvm::Function::ExternalLinkage, "__gxx_personality_v0", module.get());


        lf_incRef = createFunction("incRef", lt_void, lt_qvalue);
        lf_decRef = createFunction("decRef", lt_void, lt_qvalue);
        lf_decRefNoexcept = createFunction("decRefNoexcept", lt_void, lt_qvalue, lt_qvalue);

        lf_gv_create = createFunction("gv_create", lt_GlobalVariable_ptr, lt_qvalue);
        lf_gv_free = createFunction("gv_free", lt_qvalue, lt_GlobalVariable_ptr);
        lf_gv_read_lock = createFunction("gv_read_lock", lt_void, lt_GlobalVariable_ptr);
        lf_gv_read_unlock = createFunction("gv_read_unlock", lt_void, lt_GlobalVariable_ptr);
        lf_gv_write_lock = createFunction("gv_write_lock", lt_void, lt_GlobalVariable_ptr);
        lf_gv_write_unlock = createFunction("gv_write_unlock", lt_void, lt_GlobalVariable_ptr);
        lf_gv_get = createFunction("gv_get", lt_qvalue, lt_GlobalVariable_ptr);
        lf_gv_set = createFunction("gv_set", lt_void, lt_GlobalVariable_ptr, lt_qvalue);

        lf_qint_to_qvalue = createFunction("qint_to_qvalue", lt_qvalue, lt_qint);

        lf_createString = createFunction("createString", lt_qvalue, lt_char_ptr, lt_qsize);

        //FIXME this can be done generically using rt::meta::*Desc
        convFunctions[rt::meta::ConversionTable + static_cast<int>(rt::Conversion::IntToString)]
                      = createFunction("convertIntToString", lt_qvalue, lt_qvalue);
        convFunctions[rt::meta::ConversionTable + static_cast<int>(rt::Conversion::StringToInt)]
                      = createFunction("convertStringToInt", lt_qvalue, lt_qvalue);
        convFunctions[rt::meta::ConversionTable + static_cast<int>(rt::Conversion::BoxInt)]
                      = createFunction("int_box", lt_qvalue, lt_qvalue);

        binOpFunctions[rt::meta::BinaryOperatorTable + static_cast<int>(rt::Operator::StringPlusString)]
                       = createFunction("opAddStringString", lt_qvalue, lt_qvalue, lt_qvalue);
        binOpFunctions[rt::meta::BinaryOperatorTable + static_cast<int>(rt::Operator::IntPlusInt)]
                       = createFunction("opAddIntInt", lt_qvalue, lt_qvalue, lt_qvalue);
        binOpFunctions[rt::meta::BinaryOperatorTable + static_cast<int>(rt::Operator::AnyPlusAny)]
                       = createFunction("op_add_any_any", lt_qvalue, lt_qvalue, lt_qvalue);
        binOpFunctions[rt::meta::BinaryOperatorTable + static_cast<int>(rt::Operator::AnyPlusEqAny)]
                       = createFunction("op_addeq_any_any", lt_qvalue, lt_qvalue, lt_qvalue);
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
    llvm::Type *lt_char_ptr;
    llvm::Type *lt_int32;
    llvm::Type *lt_exc;

    llvm::Function *lf_incRef;
    llvm::Function *lf_decRef;
    llvm::Function *lf_decRefNoexcept;

    llvm::Function *lf_gv_create;
    llvm::Function *lf_gv_free;
    llvm::Function *lf_gv_read_lock;
    llvm::Function *lf_gv_read_unlock;
    llvm::Function *lf_gv_write_lock;
    llvm::Function *lf_gv_write_unlock;
    llvm::Function *lf_gv_get;
    llvm::Function *lf_gv_set;

    llvm::Function *lf_qint_to_qvalue;

    llvm::Function *lf_createString;

    llvm::Function *lf_personality;

    std::unordered_map<const rt::meta::ConversionDesc *, llvm::Function *> convFunctions;
    std::unordered_map<const rt::meta::BinaryOperatorDesc *, llvm::Function *> binOpFunctions;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_HELPER_H_
