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
/// \brief Code builder implementation
///
//------------------------------------------------------------------------------
#include "qore/comp/sem/Builder.h"
#include <vector>
#include "LValue.h"

namespace qore {
namespace comp {
namespace sem {

code::Block *Builder::getLandingPad() {
    return getLandingPad2(localsStack.rbegin());
}

code::Block *Builder::getLandingPad2(std::vector<LocalsStackItem>::reverse_iterator iit) {
    while (iit != localsStack.rend()) {
        if (iit->b) {
            break;
        }
        ++iit;
    }

    if (derefTemps.empty() && (!unlockLValue || !unlockLValue->hasLock()) && iit == localsStack.rend()) {
        return nullptr;
    }

    code::Block *savedCurrent = currentBlock;
    code::Block *block = createBlock();
    currentBlock = block;

    if (unlockLValue) {
        unlockLValue->unlock(*this);
    }

    for (auto it = derefTemps.rbegin(); it != derefTemps.rend(); ++it) {
        currentBlock->appendRefDecNoexcept(code::Temp(*it));
    }

    if (iit != localsStack.rend()) {
        currentBlock->appendJump(*iit->b);
    } else {
        currentBlock->appendResumeUnwind();
    }

    currentBlock = savedCurrent;
    return block;
}

void Builder::buildLocalsDerefForRet() {
    for (auto it = localsStack.rbegin(); it != localsStack.rend(); ++it) {
        if (it->lv && it->lv->getType().isRefCounted()) {
            TempHelper temp(*this);
            currentBlock->appendLocalGet(temp, *it->lv);
            currentBlock->appendRefDec(temp, getLandingPad2(it + 1));
        }
    }
}

void Builder::localsPush(const LocalVariableInfo &lv) {
    code::Block *b = createBlock();
    {
        TempHelper temp(*this);
        b->appendLocalGet(temp, lv.getRt());
        b->appendRefDecNoexcept(temp);
    }

    code::Block *b2 = nullptr;
    for (auto it = localsStack.rbegin(); it != localsStack.rend(); ++it) {
        if (it->b) {
            b2 = it->b;
            break;
        }
    }

    if (b2) {
        b->appendJump(*b2);
    } else {
        b->appendResumeUnwind();
    }
    localsStack.emplace_back(lv.getRt(), b);
}

} // namespace sem
} // namespace comp
} // namespace qore
