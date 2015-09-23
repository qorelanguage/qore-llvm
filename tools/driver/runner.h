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
#ifndef TOOLS_DRIVER_RUNNER_H_
#define TOOLS_DRIVER_RUNNER_H_

#include "qore/common/Util.h"
#include "qore/qil/Machine.h"

namespace qore {

template<typename Backend>
class Runner {

public:
    Runner(const qil::Script &script, Backend &backend) : script(script), backend(backend) {
        for (const auto &s : script.strings) {
            s->data = backend.createStringLiteral(s->value);
        }
        for (const auto &v : script.variables) {
            v->data = backend.createVariable(v->name);
        }
    }

    ~Runner() {
        for (const auto &v : script.variables) {
            backend.destroyVariable(static_cast<typename Backend::VariableData>(v->data));
        }
        for (const auto &s : script.strings) {
            backend.destroyStringLiteral(static_cast<typename Backend::StringLiteralData>(s->data));
        }
    }

    void run() {
        qil::Machine<Backend> r(backend);
        r.run(script.code);
    }

private:
    const qil::Script &script;
    Backend &backend;
};

template<typename Backend>
class IntScriptScope : public analyzer::Scope {

public:
    IntScriptScope(Backend &backend, qil::ScriptBuilder &scriptBuilder) : backend(backend), scriptBuilder(scriptBuilder) {
    }

    qil::Variable *declareVariable(const std::string &name, const SourceRange &range) override {
        QORE_UNREACHABLE("Should not happen");
    }

    qil::Variable *resolve(const std::string &name, const SourceRange &range) override {
        //TODO recovery must be handled by the caller, otherwise the variable will always be global
        QORE_UNREACHABLE("Undeclared " << name);
    }

    qil::StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) override {
        qil::StringLiteral *str = scriptBuilder.createStringLiteral(value, range);
        str->data = backend.createStringLiteral(value);
        return str;
    }

    qil::Variable *createVariable(const std::string &name, const SourceRange &range) override {
        qil::Variable *var = scriptBuilder.createVariable(name, range);
        var->data = backend.createVariable(name);
        return var;
    }

private:
    Backend backend;
    qil::ScriptBuilder &scriptBuilder;
};

template<typename Backend>
class IntAnalyzer {

public:
    IntAnalyzer(Backend backend = Backend()) : backend(backend), scriptScope(backend, scriptBuilder),
            scope(scriptScope, scriptBuilder.getCodeBuilder()), rrr(backend) {
    }

    ~IntAnalyzer() {
        LOG_FUNCTION();
        scope.close(SourceLocation());
        qil::Script script = scriptBuilder.build();
        rrr.run(script.code);
        for (const auto &v : script.variables) {       //TODO let destructors do this
            backend.destroyVariable(static_cast<typename Backend::VariableData>(v->data));
        }
        for (const auto &s : script.strings) {
            backend.destroyStringLiteral(static_cast<typename Backend::StringLiteralData>(s->data));
        }
    }

    void analyze(const ast::Statement::Ptr &node) {
        analyzer::script::Visitor<> visitor(scope, scriptBuilder.getCodeBuilder());
        node->accept(visitor);
        //TODO if error reported, clear code
        //TODO interactive diag manager -> throw exception, catch it here
        rrr.run(scriptBuilder.getCodeBuilder().build());
    }

private:
    Backend backend;
    qil::ScriptBuilder scriptBuilder;
    IntScriptScope<Backend> scriptScope;
    analyzer::script::BlockScope scope;
    qil::Machine<Backend> rrr;
};

} // namespace qore

#endif // TOOLS_DRIVER_RUNNER_H_
