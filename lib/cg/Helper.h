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
        lt_Context = llvm::StructType::create(ctx, "Context");
        lt_Context_ptr = lt_Context->getPointerTo();

        lt_char_ptr = llvm::Type::getInt8PtrTy(ctx);
        lt_int32 = llvm::Type::getInt32Ty(ctx);
        lt_Id = llvm::Type::getInt32Ty(ctx);
        lt_exc = llvm::StructType::get(lt_char_ptr, lt_int32, nullptr);

        lt_bool = llvm::Type::getInt1Ty(ctx);

        lf_personality = llvm::Function::Create(llvm::FunctionType::get(lt_int32, true),
                llvm::Function::ExternalLinkage, "__gxx_personality_v0", module.get());


        lf_incRef = createFunction("incRef", lt_void, lt_qvalue);
        lf_decRef = createFunction("decRef", lt_void, lt_qvalue);
        lf_decRefNoexcept = createFunction("decRefNoexcept", lt_void, lt_qvalue, lt_qvalue);

        lf_gv_read_lock = createFunction("gv_read_lock", lt_void, lt_Context_ptr, lt_Id);
        lf_gv_read_unlock = createFunction("gv_read_unlock", lt_void, lt_Context_ptr, lt_Id);
        lf_gv_write_lock = createFunction("gv_write_lock", lt_void, lt_Context_ptr, lt_Id);
        lf_gv_write_unlock = createFunction("gv_write_unlock", lt_void, lt_Context_ptr, lt_Id);
        lf_gv_get = createFunction("gv_get", lt_qvalue, lt_Context_ptr, lt_Id);
        lf_gv_set = createFunction("gv_set", lt_void, lt_Context_ptr, lt_Id, lt_qvalue);

        lf_qint_to_qvalue = createFunction("qint_to_qvalue", lt_qvalue, lt_qint);
        lf_qvalue_to_qbool = createFunction("qvalue_to_qbool", lt_bool, lt_qvalue);

        lf_createString = createFunction("createString", lt_void, lt_Context_ptr, lt_Id, lt_char_ptr, lt_qsize);
        lf_loadString = createFunction("loadString", lt_qvalue, lt_Context_ptr, lt_Id);

        lf_createGlobal = createFunction("createGlobal", lt_void, lt_Context_ptr, lt_Id, lt_bool, lt_qvalue);
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

    llvm::Value *wrapId(Id id) {
        return llvm::ConstantInt::get(lt_Id, id, false);
    }

    llvm::Value *wrapBool(bool b) {
        return llvm::ConstantInt::get(lt_bool, b, false);
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

    llvm::Value *loadString(String *string) {
        QORE_NOT_IMPLEMENTED("");
        //find entry in a map of String * -> llvm::GlobalVariable *
        //if not found:
        //      create a llvm::GlobalVariable of type String *
        //      create the mapping
        //return builder.CreateLoad(gv);
        //before the module is built, go through the mapping and create init & cleanup code for each string
        //            case comp::as::Instruction::Kind::MakeStringLiteral: {
        //                comp::as::MakeStringLiteral &ins = static_cast<comp::as::MakeStringLiteral &>(*ii);
        //                Id id = ins.getStringLiteral().getId();
        //                llvm::Constant *val = llvm::ConstantDataArray::getString(helper.ctx, ins.getValue(), true);
        //                llvm::GlobalVariable *str = new llvm::GlobalVariable(*helper.module, val->getType(), true,
        //                        llvm::GlobalValue::PrivateLinkage, val, llvm::Twine("str").concat(llvm::Twine(id)));
        //                str->setUnnamedAddr(true);
        //
        //                llvm::Value *args[4] = {
        //                        rtctx,
        //                        helper.wrapId(id),
        //                        builder.CreateConstGEP2_32(nullptr, str, 0, 0),
        //                        llvm::ConstantInt::get(helper.lt_qsize, ins.getValue().length(), false),
        //                };
        //                builder.CreateCall(helper.lf_createString, args);
        //                break;
        //            }
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
    llvm::Type *lt_Context;
    llvm::Type *lt_Context_ptr;
    llvm::Type *lt_char_ptr;
    llvm::Type *lt_int32;
    llvm::Type *lt_exc;
    llvm::Type *lt_Id;
    llvm::Type *lt_bool;

    llvm::Function *lf_incRef;
    llvm::Function *lf_decRef;
    llvm::Function *lf_decRefNoexcept;

    llvm::Function *lf_gv_read_lock;
    llvm::Function *lf_gv_read_unlock;
    llvm::Function *lf_gv_write_lock;
    llvm::Function *lf_gv_write_unlock;
    llvm::Function *lf_gv_get;
    llvm::Function *lf_gv_set;

    llvm::Function *lf_qint_to_qvalue;
    llvm::Function *lf_qvalue_to_qbool;

    llvm::Function *lf_createString;
    llvm::Function *lf_loadString;
    llvm::Function *lf_createGlobal;

    llvm::Function *lf_personality;

private:
    std::unordered_map<const Conversion *, llvm::Function *> convFunctions;
    std::unordered_map<const BinaryOperator *, llvm::Function *> binOpFunctions;
};

} // namespace cg
} // namespace qore

#endif // LIB_CG_HELPER_H_
