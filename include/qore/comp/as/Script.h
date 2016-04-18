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
#ifndef INCLUDE_QORE_COMP_AS_SCRIPT_H_
#define INCLUDE_QORE_COMP_AS_SCRIPT_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "qore/common/Util.h"
#include "qore/comp/as/Function.h"
#include "qore/comp/as/GlobalVariable.h"
#include "qore/comp/as/StringLiteral.h"

namespace qore {
namespace comp {
namespace as {

class Script {

public:
    using Ptr = std::unique_ptr<Script>;

public:
    Script(std::vector<std::unique_ptr<Type>> types, std::vector<std::unique_ptr<GlobalVariable>> globalVariables,
            std::map<std::string, std::unique_ptr<Function>> functions) : types(std::move(types)),
            globalVariables(std::move(globalVariables)), functions(std::move(functions)) {
    }

    Function &getFunction(const std::string &name) {
        auto it = functions.find(name);
        assert(it != functions.end());
        return *it->second;
    }

private:
    Script(const Script &) = delete;
    Script &operator=(const Script &) = delete;
    Script(Script &&) = delete;
    Script &operator=(Script &&) = delete;

private:
    std::vector<std::unique_ptr<Type>> types;
    std::vector<std::unique_ptr<GlobalVariable>> globalVariables;
    std::map<std::string, std::unique_ptr<Function>> functions;
};

} // namespace as
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AS_SCRIPT_H_
