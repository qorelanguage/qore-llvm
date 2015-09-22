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
    Runner(const analyzer::Script &script, Backend &backend) : script(script), backend(backend) {
        for (const auto &s : script.getStrings()) {
            s->data = backend.createStringLiteral(s->value);
        }
        for (const auto &v : script.getVariables()) {
            v->data = backend.createVariable(v->name);
        }
    }

    ~Runner() {
        for (const auto &v : script.getVariables()) {
            backend.destroyVariable(static_cast<typename Backend::VariableData>(v->data));
        }
        for (const auto &s : script.getStrings()) {
            backend.destroyStringLiteral(static_cast<typename Backend::StringLiteralData>(s->data));
        }
    }

    void run() {
        qil::Machine<Backend> r(backend);
        r.run(script.getCode());
    }

private:
    const analyzer::Script &script;
    Backend &backend;
};


template<typename Backend>
class IntAnalyzer : private analyzer::Scope {

public:
    IntAnalyzer(Backend backend = Backend()) : backend(backend), rrr(backend) {
    }

    ~IntAnalyzer() {
        for (const auto &v : variables) {       //TODO let destructors do this
            backend.destroyVariable(static_cast<typename Backend::VariableData>(v->data));
        }
        for (const auto &s : strings) {
            backend.destroyStringLiteral(static_cast<typename Backend::StringLiteralData>(s->data));
        }
    }

    void analyze(const ast::Statement::Ptr &node) {
        analyzer::script::Visitor<> visitor(*this, codeBuilder);
        node->accept(visitor);
        //TODO if error reported, clear code
        //TODO interactive diag manager -> throw exception, catch it here
        rrr.run(codeBuilder.build());
    }

    qil::Variable *createLocalVariable(const std::string &name, const SourceRange &range) override {
        qil::Variable *&var = varLookup[name];
        if (var) {
            //TODO error redeclaration
        } else {
            var = create(name, range);
            var->data = backend.createVariable(name);
            codeBuilder.lifetimeStart(range.start, var);
        }
        return var;
    }

    qil::Variable *resolve(const std::string &name, const SourceRange &range) override {
        qil::Variable *&var = varLookup[name];
        if (!var) {
            //TODO ERROR: undeclared
        }
        return var;
    }

    qil::StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) override {
        qil::StringLiteral *&s = strLookup[value];
        if (!s) {
            s = new qil::StringLiteral(value, range.start);
            strings.emplace_back(s);
            s->data = backend.createStringLiteral(value);
        }
        return s;
    }

private:
    qil::Variable *create(const std::string &name, const SourceRange &range) {
        qil::Variable *v = new qil::Variable(name, range.start);
        variables.emplace_back(v);
        return v;
    }

private:
    Backend backend;
    qil::CodeBuilder codeBuilder;
    qil::Machine<Backend> rrr;
    std::vector<std::unique_ptr<qil::Variable>> variables;
    std::map<std::string, qil::Variable *> varLookup;
    std::vector<std::unique_ptr<qil::StringLiteral>> strings;
    std::map<std::string, qil::StringLiteral *> strLookup;
};

} // namespace qore

#endif // TOOLS_DRIVER_RUNNER_H_
