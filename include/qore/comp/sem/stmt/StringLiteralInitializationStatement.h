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
/// \brief StringLiteralInitializationStatement definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_STMT_STRINGLITERALINITIALIZATIONSTATEMENT_H_
#define INCLUDE_QORE_COMP_SEM_STMT_STRINGLITERALINITIALIZATIONSTATEMENT_H_

#include <string>
#include "qore/comp/sem/stmt/Statement.h"
#include "qore/comp/as/StringLiteral.h"

namespace qore {
namespace comp {
namespace sem {

class StringLiteralInitializationStatement : public Statement {

public:
    using Ptr = std::unique_ptr<StringLiteralInitializationStatement>;

public:
    static Ptr create(as::StringLiteral stringLiteral, std::string value) {
        return Ptr(new StringLiteralInitializationStatement(stringLiteral, std::move(value)));
    }

    Kind getKind() const override {
        return Kind::StringLiteralInitialization;
    }

    as::StringLiteral getStringLiteral() const {
        return stringLiteral;
    }

    const std::string &getValue() const {
        return value;
    }

private:
    StringLiteralInitializationStatement(as::StringLiteral stringLiteral, std::string value)
            : stringLiteral(stringLiteral), value(std::move(value)) {
    }

private:
    as::StringLiteral stringLiteral;
    std::string value;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_STMT_STRINGLITERALINITIALIZATIONSTATEMENT_H_
