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
#include <vector>
#include "LValue.h"

namespace qore {
namespace comp {
namespace sem {

code::Block *Builder::getLandingPad() {
    return getLandingPad2(cleanupScopes.rbegin());
}

code::Block *Builder::getLandingPad2(std::vector<CleanupScope>::reverse_iterator iit) {
    while (iit != cleanupScopes.rend()) {
        if (iit->b) {
            break;
        }
        ++iit;
    }

    if (cleanupTemps.empty() && (!cleanupLValue || !cleanupLValue->hasLock())) {

        if (iit == cleanupScopes.rend()) {
            return nullptr;
        }

        if (iit->lpad == nullptr) {
            iit->lpad = createBlock();
            iit->lpad->appendJump(*iit->b);
        }
        return iit->lpad;
    }

    code::Block *savedCurrent = currentBlock;
    code::Block *block = createBlock();
    currentBlock = block;

    if (cleanupLValue) {
        cleanupLValue->unlock(*this);
    }

    for (auto it = cleanupTemps.rbegin(); it != cleanupTemps.rend(); ++it) {
        currentBlock->appendRefDecNoexcept(code::Temp(*it));
    }

    if (iit != cleanupScopes.rend()) {
        currentBlock->appendJump(*iit->b);
    } else {
        currentBlock->appendResumeUnwind();
    }

    currentBlock = savedCurrent;
    return block;
}

void Builder::buildCleanupForRet() {
    for (auto it = cleanupScopes.rbegin(); it != cleanupScopes.rend(); ++it) {
        if (it->lv && it->lv->getType().isRefCounted()) {
            code::Temp temp = getFreeTemp();
            currentBlock->appendLocalGet(temp, *it->lv);
            currentBlock->appendRefDec(temp, getLandingPad2(it + 1));
            setTempFree(temp);
        }
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
