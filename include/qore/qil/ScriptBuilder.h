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
#ifndef INCLUDE_QORE_QIL_SCRIPTBUILDER_H_
#define INCLUDE_QORE_QIL_SCRIPTBUILDER_H_

#include <map>
#include <string>
#include "qore/context/SourceRange.h"
#include "qore/qil/CodeBuilder.h"
#include "qore/qil/Script.h"

namespace qore {
namespace qil {

class ScriptBuilder {

public:
    Script build() {
        return Script(std::move(strings), std::move(variables), codeBuilder.build());
    }

    StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) {
        StringLiteral *&s = strLookup[value];
        if (!s) {
            s = new StringLiteral(std::move(value), range.start);
            strings.emplace_back(s);
        }
        return s;
    }

    Variable *createVariable(const std::string &name, const SourceRange &range) {
        Variable *v = new Variable(name, range.start);
        variables.emplace_back(v);
        return v;
    }

    CodeBuilder &getCodeBuilder() {
        return codeBuilder;
    }

private:
    CodeBuilder codeBuilder;
    Script::Variables variables;
    Script::Strings strings;
    std::map<std::string, qil::StringLiteral *> strLookup;
};

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_SCRIPTBUILDER_H_
