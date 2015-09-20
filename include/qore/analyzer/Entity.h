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
#ifndef INCLUDE_QORE_ANALYZER_ENTITY_H_
#define INCLUDE_QORE_ANALYZER_ENTITY_H_

#include <string>
#include "qore/context/SourceRange.h"
#include "qore/common/Logging.h"
#include "qore/common/Util.h"

namespace qore {
namespace analyzer {

#define LC_ENTITY   "ENTITY"


class LocalVariable {

public:
    LocalVariable(int id, std::string name, SourceRange range) : id(id), name(std::move(name)), range(std::move(range)) {
        CLOG(LC_ENTITY, "Creating " << *this);
    }

    ~LocalVariable() {
        CLOG(LC_ENTITY, "Destroying " << *this);
    }

    int getId() const {
        return id;
    }

    const std::string &getName() const {
        return name;
    }

private:
    friend std::ostream &operator<<(std::ostream &os, const LocalVariable &info) {
        return os << "LocalVariable " << info.name << " @" << info.range;
    }

private:
    int id;
    std::string name;
    SourceRange range;
};

class StringLiteral {

public:
    StringLiteral(int id, std::string value, SourceRange range) : id(id), value(std::move(value)), range(std::move(range)) {
        CLOG(LC_ENTITY, "Creating " << *this);
    }

    ~StringLiteral() {
        CLOG(LC_ENTITY, "Destroying " << *this);
    }

    int getId() const {
        return id;
    }

    const std::string &getValue() const {
        return value;
    }

private:
    friend std::ostream &operator<<(std::ostream &os, const StringLiteral &info) {
        return os << "StringLiteral \"" << info.value << "\" @" << info.range;
    }

private:
    int id;
    std::string value;
    SourceRange range;
};

} // namespace analyzer
} // namespace qore

#include "qore/analyzer/Code.h"

namespace qore {
namespace analyzer {

class Script {

public:
    Script(std::vector<std::unique_ptr<StringLiteral>> strings, std::vector<std::unique_ptr<LocalVariable>> variables, Code code) : strings(std::move(strings)), variables(std::move(variables)), code(std::move(code)) {
    }

    const std::vector<std::unique_ptr<StringLiteral>> &getStrings() const {
        return strings;
    }

    const std::vector<std::unique_ptr<LocalVariable>> &getVariables() const {
        return variables;
    }

    const Code &getCode() const {
        return code;
    }

private:
    std::vector<std::unique_ptr<StringLiteral>> strings;
    std::vector<std::unique_ptr<LocalVariable>> variables;
    Code code;
};

#undef LC_ENTITY

} // namespace analyzer
} // namespace qore

#endif // INCLUDE_QORE_ANALYZER_ENTITY_H_
