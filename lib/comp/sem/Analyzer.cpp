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

Analyzer::Analyzer(Context &ctx, AnalyzerCallbacks &callbacks, as::Script &script) : Core(ctx), callbacks(callbacks),
        script(script), rootNamespace(*this) {
    builtinTypes[ctx.getStringTable().put("int")] = std::make_pair(&as::Type::Int, &as::Type::IntOpt);
    builtinTypes[ctx.getStringTable().put("string")] = std::make_pair(&as::Type::String, &as::Type::StringOpt);
    builtinTypes[ctx.getStringTable().put("any")] = std::make_pair(&as::Type::Any, nullptr);
}

void Analyzer::processDeclaration(ast::Declaration &decl) {
    try {
        rootNamespace.processDeclaration(decl);
    } catch (ReportedError &) {
        // ignored, diagnostic has been reported already
    }
}

void Analyzer::processPendingDeclarations() {
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

    for (auto it : functionQueue) {
        doPass1(*it.first, *it.second);
        //doPass2
        //finalize function scope
    }
    functionQueue.clear();
}

const as::Type &Analyzer::resolveType(const NamespaceScope &scope, const ast::Type &node) const {
    if (node.getKind() == ast::Type::Kind::Implicit) {
        //return nothing/void
        QORE_NOT_IMPLEMENTED("");
    }
    if (node.getKind() == ast::Type::Kind::Invalid || !node.getName().isValid()) {
        return as::Type::Error;
    }

    bool asterisk;
    if (node.getKind() == ast::Type::Kind::Asterisk) {
        asterisk = true;
    } else {
        assert(node.getKind() == ast::Type::Kind::Basic);
        asterisk = false;
    }

    if (node.getName().isSimple()) {
        auto it = builtinTypes.find(node.getName().last().str);
        if (it != builtinTypes.end()) {
            if (asterisk && !it->second.second) {
                //FIXME report error (any with asterisk)
                return as::Type::Error;
            }
            return asterisk ? *it->second.second : *it->second.first;
        }
    }

    try {
        ClassScope &c = scope.resolveClass(node.getName());
        return asterisk ? *c.getTypes().second : *c.getTypes().first;
    } catch (ReportedError &) {
        return as::Type::Error;
    }
}

Statement::Ptr Analyzer::doPass1(BlockScope &scope, ast::Statement &stmt) {
    StatementAnalyzerPass1 a(*this, scope);
    return stmt.accept(a);
}

void Analyzer::doPass2(Builder &builder, Statement &stmt) {
    StatementAnalyzerPass2 a(*this, builder);
    stmt.accept(a);
}

as::GlobalVariable &Analyzer::createGlobalVariable(String::Ref name, SourceLocation location, const as::Type &type) {
    as::GlobalVariable &gv = script.createGlobalVariable(name, location, type);
    Expression::Ptr value = ExpressionAnalyzerPass1::defaultFor(*this, type);
    callbacks.addInitializer(GlobalVariableInitializationStatement::create(gv, std::move(value)));
    return gv;
}

as::StringLiteral Analyzer::createStringLiteral(const std::string &value) {
    auto it = strings.find(value);
    if (it != strings.end()) {
        return it->second;
    }
    as::StringLiteral sl = script.createStringLiteral();
    strings.insert(std::make_pair(value, sl));
    callbacks.addInitializer(StringLiteralInitializationStatement::create(sl, std::move(value)));
    return sl;
}

} // namespace sem
} // namespace comp
} // namespace qore
