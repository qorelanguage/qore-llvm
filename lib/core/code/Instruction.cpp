//--------------------------------------------------------------------*- C++ -*-
//
//  Qore Programming Language
//
//  Copyright (C) 2015 - 2020 Qore Technologies, s.r.o.
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
/// \brief Implementation of an Instruction.
///
//------------------------------------------------------------------------------
/*
#include "qore/core/code/Instruction.h"
#include "qore/core/code/Branch.h"
#include "qore/core/code/ConstInt.h"
#include "qore/core/code/ConstNothing.h"
#include "qore/core/code/ConstString.h"
#include "qore/core/code/GlobalGet.h"
#include "qore/core/code/GlobalInit.h"
#include "qore/core/code/GlobalReadLock.h"
#include "qore/core/code/GlobalReadUnlock.h"
#include "qore/core/code/GlobalSet.h"
#include "qore/core/code/GlobalWriteLock.h"
#include "qore/core/code/GlobalWriteUnlock.h"
#include "qore/core/code/InvokeBinaryOperator.h"
#include "qore/core/code/InvokeConversion.h"
#include "qore/core/code/InvokeFunction.h"
#include "qore/core/code/Jump.h"
#include "qore/core/code/LocalGet.h"
#include "qore/core/code/LocalSet.h"
#include "qore/core/code/RefDec.h"
#include "qore/core/code/RefDecNoexcept.h"
#include "qore/core/code/RefInc.h"
#include "qore/core/code/ResumeUnwind.h"
#include "qore/core/code/Ret.h"
#include "qore/core/code/RetVoid.h"
*/
namespace qore {
namespace code {

/**
 * \brief Calls visitor's `visit()` method appropriate for the concrete type of the Instruction.
 * \param visitor the visitor to call
 * \return the value returned from the visitor
 * \tparam V the type of the visitor
 */

/*
template<typename V>
typename V::ReturnType Instruction::accept(V &visitor) const {
    /// \cond NoDoxygen
    #define CASE(K) case Kind::K: return visitor.visit(static_cast<const K &>(*this));
    /// \endcond NoDoxygen
    switch (getKind()) {
        CASE(Branch);
        CASE(ConstInt);
        CASE(ConstNothing);
        CASE(ConstString);
        CASE(GlobalGet);
        CASE(GlobalInit);
        CASE(GlobalReadLock);
        CASE(GlobalReadUnlock);
        CASE(GlobalSet);
        CASE(GlobalWriteLock);
        CASE(GlobalWriteUnlock);
        CASE(InvokeBinaryOperator);
        CASE(InvokeConversion);
        CASE(InvokeFunction);
        CASE(Jump);
        CASE(LocalGet);
        CASE(LocalSet);
        CASE(RefDec);
        CASE(RefDecNoexcept);
        CASE(RefInc);
        CASE(ResumeUnwind);
        CASE(Ret);
        CASE(RetVoid);
        default:
            QORE_UNREACHABLE("Invalid Instruction::Kind: " << static_cast<int>(getKind()));
    }
    #undef CASE
}
*/

} //namespace code
} //namespace qore
