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
#include "qore/core/util/Debug.h"
#include "qore/core/util/Util.h"

namespace qore {
namespace cg {

class Helper {

public:
    Helper() : ctx(llvm::getGlobalContext()) {
        module = util::make_unique<llvm::Module>("Q", ctx);

        //basic ("C") types
        lt_void = llvm::Type::getVoidTy(ctx);
        lt_void_ptr = llvm::Type::getInt8PtrTy(ctx);
        lt_bool = llvm::Type::getInt1Ty(ctx);
        lt_char_ptr = llvm::Type::getInt8PtrTy(ctx);
        lt_int32 = llvm::Type::getInt32Ty(ctx);
        lt_exc = llvm::StructType::get(lt_char_ptr, lt_int32, nullptr);

        //Qore core types
        lt_qint = llvm::Type::getInt64Ty(ctx);
        lt_qvalue = llvm::StructType::create(lt_qint, "qvalue", false);

        lt_Env_ptr = llvm::StructType::create(ctx, "::qore::Env")->getPointerTo();
        lt_FunctionGroup_ptr = llvm::StructType::create(ctx, "::qore::FunctionGroup")->getPointerTo();
        lt_Function_ptr = llvm::StructType::create(ctx, "::qore::Function")->getPointerTo();
        lt_GlobalVariable_ptr = llvm::StructType::create(ctx, "::qore::GlobalVariable")->getPointerTo();
        lt_Namespace_ptr = llvm::StructType::create(ctx, "::qore::Namespace")->getPointerTo();
        lt_SourceInfo_ptr = llvm::StructType::create(ctx, "::qore::SourceInfo")->getPointerTo();
        lt_Type_ptr = llvm::StructType::create(ctx, "::qore::Type")->getPointerTo();

        //personality function
        lf_personality = llvm::Function::Create(llvm::FunctionType::get(lt_int32, true),
                llvm::Function::ExternalLinkage, "__gxx_personality_v0", module.get());

        //nrt utility functions
        lf_qint_to_qvalue = createFunction("qint_to_qvalue", lt_qvalue, lt_qint);
        lf_qvalue_to_qbool = createFunction("qvalue_to_qbool", lt_bool, lt_qvalue);

        //nrt wrappers for Env
        lf_env_getRootNamespace = createFunction("env_getRootNamespace", lt_Namespace_ptr, lt_Env_ptr);
        lf_env_addSourceInfo = createFunction("env_addSourceInfo", lt_SourceInfo_ptr, lt_Env_ptr, lt_char_ptr);
        lf_env_addString = createFunction("env_addString", lt_qvalue, lt_Env_ptr, lt_char_ptr);

        //nrt wrappers for Namespace
        lf_namespace_addNamespace = createFunction("namespace_addNamespace",
                lt_Namespace_ptr, lt_Namespace_ptr, lt_char_ptr, lt_SourceInfo_ptr, lt_int32);
        lf_namespace_addFunctionGroup = createFunction("namespace_addFunctionGroup",
                lt_FunctionGroup_ptr, lt_Namespace_ptr, lt_char_ptr);
        lf_namespace_addGlobalVariable = createFunction("namespace_addGlobalVariable",
                lt_GlobalVariable_ptr, lt_Namespace_ptr, lt_char_ptr, lt_Type_ptr, lt_SourceInfo_ptr, lt_int32);

        //nrt wrappers for GlobalVariable
        lf_globalVariable_initValue = createFunction("globalVariable_initValue",
                lt_void, lt_GlobalVariable_ptr, lt_qvalue);
        lf_globalVariable_setValue = createFunction("globalVariable_setValue",
                lt_void, lt_GlobalVariable_ptr, lt_qvalue);
        lf_globalVariable_getValue = createFunction("globalVariable_getValue", lt_qvalue, lt_GlobalVariable_ptr);
        lf_globalVariable_readLock = createFunction("globalVariable_readLock", lt_void, lt_GlobalVariable_ptr);
        lf_globalVariable_readUnlock = createFunction("globalVariable_readUnlock", lt_void, lt_GlobalVariable_ptr);
        lf_globalVariable_writeLock = createFunction("globalVariable_writeLock", lt_void, lt_GlobalVariable_ptr);
        lf_globalVariable_writeUnlock = createFunction("globalVariable_writeUnlock", lt_void, lt_GlobalVariable_ptr);

        //nrt wrappers for Type
        lf_type_String = createFunction("type_String", lt_Type_ptr);

        //nrt instruction implementations
        lf_ref_dec = createFunction("ref_dec", lt_void, lt_qvalue);
        lf_ref_dec_noexcept = createFunction("ref_dec_noexcept", lt_void, lt_qvalue);
        lf_ref_inc = createFunction("ref_inc", lt_void, lt_qvalue);
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

    llvm::Function *createFunction(const std::string &name, llvm::Type *ret, llvm::Type *arg1, llvm::Type *arg2,
            llvm::Type *arg3) {
        llvm::Type *args[3] = {arg1, arg2, arg3};
        return llvm::Function::Create(
                llvm::FunctionType::get(ret, args, false),
                llvm::Function::ExternalLinkage, name, module.get());
    }

    llvm::Function *createFunction(const std::string &name, llvm::Type *ret, llvm::Type *arg1, llvm::Type *arg2,
            llvm::Type *arg3, llvm::Type *arg4) {
        llvm::Type *args[4] = {arg1, arg2, arg3, arg4};
        return llvm::Function::Create(
                llvm::FunctionType::get(ret, args, false),
                llvm::Function::ExternalLinkage, name, module.get());
    }

    llvm::Function *createFunction(const std::string &name, llvm::Type *ret, llvm::Type *arg1, llvm::Type *arg2,
            llvm::Type *arg3, llvm::Type *arg4, llvm::Type *arg5) {
        llvm::Type *args[5] = {arg1, arg2, arg3, arg4, arg5};
        return llvm::Function::Create(
                llvm::FunctionType::get(ret, args, false),
                llvm::Function::ExternalLinkage, name, module.get());
    }

    llvm::Function *getConversion(const Conversion &conversion) {
        llvm::Function *&ref = convFunctions[&conversion];
        if (!ref) {
            ref = createFunction(conversion.getFunctionName(), lt_qvalue, lt_qvalue);
        }
        return ref;
    }

    llvm::Function *getBinaryOperator(const BinaryOperator &op) {
        llvm::Function *&ref = binOpFunctions[&op];
        if (!ref) {
            ref = createFunction(op.getFunctionName(), lt_qvalue, lt_qvalue, lt_qvalue);
        }
        return ref;
    }

public:
    llvm::LLVMContext &ctx;
    std::unique_ptr<llvm::Module> module;

    llvm::Type *lt_void;
    llvm::Type *lt_void_ptr;
    llvm::Type *lt_bool;
    llvm::Type *lt_char_ptr;
    llvm::Type *lt_int32;
    llvm::Type *lt_exc;

    llvm::Type *lt_qint;
    llvm::Type *lt_qvalue;
    llvm::Type *lt_Env_ptr;
    llvm::Type *lt_Function_ptr;
    llvm::Type *lt_FunctionGroup_ptr;
    llvm::Type *lt_GlobalVariable_ptr;
    llvm::Type *lt_Namespace_ptr;
    llvm::Type *lt_SourceInfo_ptr;
    llvm::Type *lt_Type_ptr;

    llvm::Function *lf_personality;

    llvm::Function *lf_qint_to_qvalue;
    llvm::Function *lf_qvalue_to_qbool;

    llvm::Function *lf_env_getRootNamespace;
    llvm::Function *lf_env_addSourceInfo;
    llvm::Function *lf_env_addString;

    llvm::Function *lf_namespace_addNamespace;
    llvm::Function *lf_namespace_addFunctionGroup;
    llvm::Function *lf_namespace_addGlobalVariable;
    llvm::Function *lf_globalVariable_initValue;
    llvm::Function *lf_globalVariable_setValue;
    llvm::Function *lf_globalVariable_getValue;
    llvm::Function *lf_globalVariable_readLock;
    llvm::Function *lf_globalVariable_readUnlock;
    llvm::Function *lf_globalVariable_writeLock;
    llvm::Function *lf_globalVariable_writeUnlock;

    llvm::Function *lf_type_String;

    llvm::Function *lf_ref_dec;
    llvm::Function *lf_ref_dec_noexcept;
    llvm::Function *lf_ref_inc;

private:
    std::unordered_map<const Conversion *, llvm::Function *> convFunctions;
    std::unordered_map<const BinaryOperator *, llvm::Function *> binOpFunctions;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_HELPER_H_
