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

bool Builder::needsLandingPad(std::vector<CleanupScope>::reverse_iterator it) {
    if (!cleanupTemps.empty() || (cleanupLValue && cleanupLValue->hasLock())) {
        return true;
    }
    while (it != cleanupScopes.rend()) {
        if (it->lv && !it->lv->getType().isPrimitive()) {
            return true;
        }
        //break if catch block?
        ++it;
    }
    return false;
}

as::Block *Builder::getLandingPad() {
    return getLandingPad2(cleanupScopes.rbegin());
}

as::Block *Builder::getLandingPad2(std::vector<CleanupScope>::reverse_iterator iit) {
    if (!needsLandingPad(iit)) {
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

    while (iit != cleanupScopes.rend()) {
        if (iit->lv && !iit->lv->getType().isPrimitive()) {
            as::Temp temp = getFreeTemp();
            createGetLocal(temp, findLocalSlot(*iit->lv));
            createRefDecNoexcept(temp, e);
            setTempFree(temp);
        }
        //if catch, create jump and break
        ++iit;
    }

    if (!terminated) {
        createRethrow(e);
    }

    setTempFree(e);
    currentBlock = savedCurrent;
    terminated = savedTeminated;
    return block;
}

void Builder::buildCleanupForRet() {
    assert(!cleanupLValue);
    assert(cleanupTemps.empty());

    for (auto it = cleanupScopes.rbegin(); it != cleanupScopes.rend(); ++it) {
        if (it->lv && !it->lv->getType().isPrimitive()) {
            as::Temp temp = getFreeTemp();
            createGetLocal(temp, findLocalSlot(*it->lv));
            add(util::make_unique<as::RefDec>(temp, getLandingPad2(it + 1)));
            setTempFree(temp);
        }
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
