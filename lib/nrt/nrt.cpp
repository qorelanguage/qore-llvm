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
/// \brief Implementation of 'native runtime' functions.
///
//------------------------------------------------------------------------------
#include "qore/nrt/nrt.h"
#include "qore/core/Env.h"

namespace qore {
namespace nrt {

// cppcheck-suppress unusedFunction
Namespace *env_getRootNamespace(Env *env) {
    return &env->getRootNamespace();
}

// cppcheck-suppress unusedFunction
SourceInfo *env_addSourceInfo(Env *env, const char *name) {
    return &env->addSourceInfo(name);
}

// cppcheck-suppress unusedFunction
qvalue env_addString(Env *env, const char *str) {
    qvalue v;
    v.p = &env->addString(str);
    return v;
}

// cppcheck-suppress unusedFunction
Namespace *namespace_addNamespace(Namespace *ns, const char *name, SourceInfo *sourceInfo, int location) {
    return &ns->addNamespace(name, SourceLocation(*sourceInfo, location));
}

// cppcheck-suppress unusedFunction
FunctionGroup *namespace_addFunctionGroup(Namespace *ns, const char *name) {
    return &ns->addFunctionGroup(name);
}

// cppcheck-suppress unusedFunction
GlobalVariable *namespace_addGlobalVariable(Namespace *ns, const char *name, const Type *type,
        SourceInfo *sourceInfo, int location) {
    return &ns->addGlobalVariable(name, *type, SourceLocation(*sourceInfo, location));
}

// cppcheck-suppress unusedFunction
void globalVariable_initValue(GlobalVariable *gv, qvalue value) {
    gv->initValue(value);
}

// cppcheck-suppress unusedFunction
void globalVariable_setValue(GlobalVariable *gv, qvalue value) {
    gv->setValue(value);
}

// cppcheck-suppress unusedFunction
qvalue globalVariable_getValue(GlobalVariable *gv) {
    return gv->getValue();
}

// cppcheck-suppress unusedFunction
void globalVariable_readLock(GlobalVariable *gv) {
    gv->readLock();
}

// cppcheck-suppress unusedFunction
void globalVariable_readUnlock(GlobalVariable *gv) {
    gv->readUnlock();
}

// cppcheck-suppress unusedFunction
void globalVariable_writeLock(GlobalVariable *gv) {
    gv->writeLock();
}

// cppcheck-suppress unusedFunction
void globalVariable_writeUnlock(GlobalVariable *gv) {
    gv->writeUnlock();
}

// cppcheck-suppress unusedFunction
const Type *type_String() {
    return &Type::String;
}

// cppcheck-suppress unusedFunction
void ref_dec(qvalue value) {
    if (value.p) {
        value.p->decRefCount();
        //FIXME if an exception was created, throw it
    }
}

// cppcheck-suppress unusedFunction
void ref_dec_noexcept(qvalue value) {
    if (value.p) {
        value.p->decRefCount();
    }
}

// cppcheck-suppress unusedFunction
void ref_inc(qvalue value) {
    if (value.p) {
        value.p->incRefCount();
    }
}

// cppcheck-suppress unusedFunction
qvalue qint_to_qvalue(qint i) {
    qvalue v;
    v.i = i;
    return v;
}

// cppcheck-suppress unusedFunction
qbool qvalue_to_qbool(qvalue v) {
    return v.b;
}

} // namespace nrt
} // namespace qore
