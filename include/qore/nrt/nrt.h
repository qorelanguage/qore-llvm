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
/// \brief Defines the 'native runtime' namespace.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_NRT_NRT_H_
#define INCLUDE_QORE_NRT_NRT_H_

#include "qore/core/Value.h"

namespace qore {

class Env;
class FunctionGroup;
class GlobalVariable;
class Namespace;
class SourceInfo;
class Type;

/**
 * \brief The 'native runtime' namespace, provides `extern "C"` wrappers for core Qore classes.
 */
namespace nrt {

///\name Wrappers for Env methods
///\{
/**
 * \brief Wraps the \ref Env::getRootNamespace() method.
 * \param env `this` for the method invocation
 * \return the root namespace
 */
extern "C" Namespace *env_getRootNamespace(Env *env);

/**
 * \brief Wraps the ref Env::addSourceInfo() method.
 * \param env `this` for the method invocation
 * \param name the name of the source
 * \return created \ref SourceInfo instance
 */
extern "C" SourceInfo *env_addSourceInfo(Env *env, const char *name);

/**
 * \brief Wraps the ref Env::addString() method.
 * \param env `this` for the method invocation
 * \param str the value of the string literal
 * \return created \ref String instance
 */
extern "C" qvalue env_addString(Env *env, const char *str);
///\}

///\name Wrappers for Namespace methods
///\{
/**
 * \brief Wraps the \ref Namespace::addNamespace() method.
 * \param ns `this` for the method invocation
 * \param name the name of the new namespace
 * \param sourceInfo identifies the script
 * \param location encoded location
 * \return the new namespace
 */
extern "C" Namespace *namespace_addNamespace(Namespace *ns, const char *name,
        SourceInfo *sourceInfo, int location);

/**
 * \brief Wraps the \ref Namespace::addFunctionGroup() method.
 * \param ns `this` for the method invocation
 * \param name the name of the new function group
 * \return the new function group
 */
extern "C" FunctionGroup *namespace_addFunctionGroup(Namespace *ns, const char *name);

/**
 * \brief Wraps the \ref Namespace::addGlobalVariable() method.
 * \param ns `this` for the method invocation
 * \param name the name of the global variable
 * \param type the type of the global variable
 * \param sourceInfo identifies the script
 * \param location encoded location
 * \return the new global variable
 */
extern "C" GlobalVariable *namespace_addGlobalVariable(Namespace *ns, const char *name, const Type *type,
        SourceInfo *sourceInfo, int location);
///\}

///\name Wrappers for GlobalVariable methods
///\{
/**
 * \brief Wraps the \ref GlobalVariable::initValue() method.
 * \param gv `this` for the method invocation
 * \param value the initial value of the global variable
 */
extern "C" void globalVariable_initValue(GlobalVariable *gv, qvalue value);

/**
 * \brief Wraps the \ref GlobalVariable::setValue() method.
 * \param gv `this` for the method invocation
 * \param value the new value of the global variable
 */
extern "C" void globalVariable_setValue(GlobalVariable *gv, qvalue value);

/**
 * \brief Wraps the \ref GlobalVariable::getValue() method.
 * \param gv `this` for the method invocation
 * \return the value of the global variable
 */
extern "C" qvalue globalVariable_getValue(GlobalVariable *gv);

/**
 * \brief Wraps the \ref GlobalVariable::readLock() method.
 * \param gv `this` for the method invocation
 */
extern "C" void globalVariable_readLock(GlobalVariable *gv);

/**
 * \brief Wraps the \ref GlobalVariable::readUnlock() method.
 * \param gv `this` for the method invocation
 */
extern "C" void globalVariable_readUnlock(GlobalVariable *gv);

/**
 * \brief Wraps the \ref GlobalVariable::writeLock() method.
 * \param gv `this` for the method invocation
 */
extern "C" void globalVariable_writeLock(GlobalVariable *gv);

/**
 * \brief Wraps the \ref GlobalVariable::writeUnlock() method.
 * \param gv `this` for the method invocation
 */
extern "C" void globalVariable_writeUnlock(GlobalVariable *gv);
///\}

///\name Wrappers for static fields of the Type class
///\{
/**
 * \brief Returns \ref Type::String.
 * \return \ref Type::String
 */
extern "C" const Type *type_String();
///\}

///\name Instruction implementations
///\{
/**
 * \brief Implements the \ref code::RefDec instruction.
 * \param value the argument of the instruction
 */
extern "C" void ref_dec(qvalue value);

/**
 * \brief Implements the \ref code::RefDecNoexcept instruction.
 * \param value the argument of the instruction
 */
extern "C" void ref_dec_noexcept(qvalue value);

/**
 * \brief Implements the \ref code::RefInc instruction.
 * \param value the argument of the instruction
 */
extern "C" void ref_inc(qvalue value);
///\}

///\name Utility functions
///\{
/**
 * \brief Converts \ref qint to \ref qvalue.
 * \param i the value to convert
 * \return converted value
 */
extern "C" qvalue qint_to_qvalue(qint i);

/**
 * \brief Converts \ref qvalue to \ref qbool.
 * \param v the value to convert
 * \return converted value
 */
extern "C" qbool qvalue_to_qbool(qvalue v);
///\}

} // namespace nrt
} // namespace qore

#endif // INCLUDE_QORE_NRT_NRT_H_
