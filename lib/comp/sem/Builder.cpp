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
#include "qore/comp/sem/Builder.h"
#include "LValue.h"

namespace qore {
namespace comp {
namespace sem {

as::Block *Builder::getLandingPad() {
    if (cleanupTemps.empty() && (!cleanupLValue || !cleanupLValue->hasLock()) && cleanupLocals.empty()) {
        return nullptr;
    }
    bool savedTeminated = terminated;
    as::Block *savedCurrent = currentBlock;
    as::Block *block = createBlock();
    currentBlock = block;
    terminated = false;

    as::Temp e = getFreeTemp();
    createLandingPad(e);

    if (cleanupLValue) {
        cleanupLValue->cleanup(*this);
    }

    for (auto it = cleanupTemps.rbegin(); it != cleanupTemps.rend(); ++it) {
        createRefDecNoexcept(as::Temp(*it), e);
    }

    for (auto it = cleanupLocals.rbegin(); it != cleanupLocals.rend(); ++it) {
        as::Temp temp = getFreeTemp();
        createGetLocal(temp, findLocalSlot(**it));
        createRefDecNoexcept(temp, e);
        setTempFree(temp);
    }

    createRethrow(e);
    setTempFree(e);
    currentBlock = savedCurrent;
    terminated = savedTeminated;
    return block;
}

void Builder::buildCleanupForRet() {
    assert(!cleanupLValue);
    assert(cleanupTemps.empty());

    for (auto it = cleanupLocals.rbegin(); it != cleanupLocals.rend(); ++it) {
        as::Temp temp = getFreeTemp();
        createGetLocal(temp, findLocalSlot(**it));
        createRefDec(temp);
        setTempFree(temp);
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
