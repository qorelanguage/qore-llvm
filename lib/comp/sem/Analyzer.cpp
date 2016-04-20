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
#include "qore/comp/sem/Analyzer.h"
#include <string>
#include "qore/comp/sem/ClassScope.h"
#include "qore/comp/sem/stmt/GlobalVariableInitializationStatement.h"
#include "qore/comp/sem/stmt/StringLiteralInitializationStatement.h"
#include "ReportedError.h"
#include "StatementAnalyzerPass1.h"
#include "StatementAnalyzerPass2.h"

namespace qore {
namespace comp {
namespace sem {

void Core::processPendingDeclarations() {
    /*
     * for each class in class queue:
     *  - resolve and process superclasses
     */
    classQueue.clear();

    for (auto it : globalVariableQueue) {
        it->pass2();
    }
    globalVariableQueue.clear();

    for (auto it : functionOverloadPackQueue) {
        it->pass2();
    }
    functionOverloadPackQueue.clear();

    /*
     * process const-queue (we need their types which means that the initializer expressions need to be compiled
     *      but there may be cycles which need to be detected)
     */

    for (auto f : functionQueue) {
        f->analyze();
    }
    functionQueue.clear();
}

Statement::Ptr Core::doPass1(Scope &scope, ast::Statement &stmt) {
    StatementAnalyzerPass1 a(*this, scope);
    return stmt.accept(a);
}

/*
 * transforms ir to as (exception safety, temporaries, ...)
 */
void Core::doPass2(Builder &builder, Statement &stmt) {
    StatementAnalyzerPass2 a(*this, builder);
    stmt.accept(a);
}

const as::Type &ScriptBuilder::getClassType(const ClassScope &c, bool asterisk) {
    auto it = classTypes.find(&c);
    if (it != classTypes.end()) {
        return asterisk ? *it->second.second : *it->second.first;
    }
    const as::Type &t1 = createType(rt::Type::Object, c.getFullName(), false);
    const as::Type &t2 = createType(rt::Type::Object, c.getFullName(), true);
    classTypes[&c] = std::make_pair(&t1, &t2);
    return asterisk ? t2 : t1;
}

as::Function &ScriptBuilder::createFunction(std::string name, Id argCount, const as::Type &retType,
        FunctionBuilder &b) {
    std::unique_ptr<as::Function> ptr
        = util::make_unique<as::Function>(name, argCount, retType, b.tempCount, b.localCount, b.clear());
    as::Function &f = *ptr;
    functions.push_back(std::move(ptr));
    return f;
}

} // namespace sem
} // namespace comp
} // namespace qore
