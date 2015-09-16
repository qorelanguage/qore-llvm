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
/// \brief TODO File description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_ANALYZER_SCOPEIMPL_H_
#define INCLUDE_QORE_ANALYZER_SCOPEIMPL_H_

#include <map>
#include "qore/analyzer/Scope.h"

namespace qore {
namespace analyzer {

template<typename Backend>
class ScopeImpl : public Scope<Backend> {
public:
    using LValue = typename Backend::LValue;

    ScopeImpl(Backend &backend) : backend(backend) {
    }

    void close() override {
        scope.clear();
    }

    LValue createLocalVariable(const std::string &name, const SourceRange &range) override {
        //TODO exception safety, duplicates etc.
        LValue var = backend.createLocalVariable(name, range);
        scope[name] = var;
        return var;
    }

    LValue resolve(const std::string &name, const SourceRange &range) override {
        LValue &var = scope[name];
        if (!var) {
            //TODO ERROR: undeclared
            return createLocalVariable(name, range);
        } else {
            return var;
        }
    }

private:
    Backend &backend;
    std::map<std::string, LValue> scope;
    //TODO order of destruction
};

} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_SCOPEIMPL_H_
