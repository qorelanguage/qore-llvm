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

#ifndef INCLUDE_QORE_ANALYZER_SCOPE_H_
#define INCLUDE_QORE_ANALYZER_SCOPE_H_

#include "qore/analyzer/Entity.h"

namespace qore {
namespace analyzer {

class Scope {

public:
    virtual ~Scope() = default;

    //TODO predavat pouze id
    //TODO LocalVariable -> Variable
    virtual qil::Variable *createLocalVariable(const std::string &name, const SourceRange &range) = 0;
    virtual qil::Variable *resolve(const std::string &name, const SourceRange &range) = 0;

    virtual qil::StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) = 0;


    //TODO virtual void add(Ins ) = 0; ???
};

} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_SCOPE_H_
