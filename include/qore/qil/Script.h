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
/// \brief Script definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_QIL_SCRIPT_H_
#define INCLUDE_QORE_QIL_SCRIPT_H_

#include <memory>
#include <vector>
#include "qore/qil/Code.h"

namespace qore {
namespace qil {

/**
 * \brief Describes an analyzed script.
 */
class Script {

public:
    using Strings = std::vector<std::unique_ptr<StringLiteral>>;
    using Variables = std::vector<std::unique_ptr<Variable>>;

public:
    Strings strings;
    Variables variables;
    Code code;

public:
    Script(Strings strings, Variables variables, Code code) : strings(std::move(strings)),
            variables(std::move(variables)), code(std::move(code)) {
    }
};

} // namespace qil
} // namespace qore

#endif // INCLUDE_QORE_QIL_SCRIPT_H_
