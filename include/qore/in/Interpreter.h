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
/// \brief Interpreter definition.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_IN_INTERPRETER_H_
#define INCLUDE_QORE_IN_INTERPRETER_H_

#include <unordered_map>
#include <vector>
#include "qore/in/Locals.h"
#include "qore/ir/decl/Function.h"
#include "qore/ir/decl/GlobalVariable.h"
#include "qore/ir/decl/Script.h"
#include "qore/ir/decl/StringLiteral.h"
#include "qore/ir/stmt/Statement.h"
#include "qore/rt/Types.h"

namespace qore {
namespace in {

class Interpreter {

public:
    Interpreter();
    ~Interpreter();

    void addStringLiteral(const ir::StringLiteral &sl);
    void addGlobalVariable(const ir::GlobalVariable &gv);
    void execute(Locals &locals, const ir::Statement &stmt);

    rt::GlobalVariable *getGlobalVariableValue(const ir::GlobalVariable &gv) const;
    rt::qvalue getStringLiteralValue(const ir::StringLiteral &sl) const;

    static void run(const ir::Script &script, const ir::UserFunction &f);

private:
    std::unordered_map<const ir::StringLiteral *, rt::qvalue> strings;
    std::unordered_map<const ir::GlobalVariable *, rt::GlobalVariable *> globals;
};

} // namespace in
} // namespace qore

#endif // INCLUDE_QORE_IN_INTERPRETER_H_