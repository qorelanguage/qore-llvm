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

Namespace *env_getRootNamespace(Env *env) {
    return &env->getRootNamespace();
}

qvalue env_addString(Env *env, const char *str) {
    qvalue v;
    v.p = env->addString(str);
    return v;
}

Namespace *namespace_addNamespace(Namespace *ns, const char *name) {
    return &ns->addNamespace(name, SourceLocation());
}

GlobalVariable *namespace_addGlobalVariable(Namespace *ns, const char *name, const Type *type) {
    return &ns->addGlobalVariable(name, *type, SourceLocation());
}

void globalVariable_initValue(GlobalVariable *gv, qvalue value) {
    gv->initValue(value);
}

void globalVariable_setValue(GlobalVariable *gv, qvalue value) {
    gv->setValue(value);
}

qvalue globalVariable_getValue(GlobalVariable *gv) {
    return gv->getValue();
}

void globalVariable_readLock(GlobalVariable *gv) {
    gv->readLock();
}

void globalVariable_readUnlock(GlobalVariable *gv) {
    gv->readUnlock();
}

void globalVariable_writeLock(GlobalVariable *gv) {
    gv->writeLock();
}

void globalVariable_writeUnlock(GlobalVariable *gv) {
    gv->writeUnlock();
}

const Type *type_String() {
    return &Type::String;
}

void ref_dec(qvalue value) {
    if (value.p) {
        value.p->decRefCount();
        //FIXME if an exception was created, throw it
    }
}

void ref_dec_noexcept(qvalue value) {
    if (value.p) {
        value.p->decRefCount();
    }
}

void ref_inc(qvalue value) {
    if (value.p) {
        value.p->incRefCount();
    }
}

qvalue qint_to_qvalue(qint i) {
    qvalue v;
    v.i = i;
    return v;
}

qbool qvalue_to_qbool(qvalue v) {
    return v.b;
}

} // namespace nrt
} // namespace qore
