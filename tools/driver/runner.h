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

namespace qore {

template<typename Backend>
class RunnerBase {

public:
    RunnerBase(Backend &backend = Backend()) : backend(backend) {
    }

    virtual ~RunnerBase() {
    }

    void run(const qil::Code &code) {
        std::deque<typename Backend::Value> stack;
        typename Backend::LValue lValReg = nullptr;
        for (const auto &i : code) {
            std::cout << "LVALREG: " << lValReg << "\n";
            std::cout << "Stack:\n";
            for (const auto &v : stack) {
                std::cout << "  " << v << "\n";
            }
            std::cout << "------" << i << "\n";
            switch (i.opcode) {
                case qil::Opcode::LifetimeStart:
                    i.variable->data = backend.createLocalVariable(i.variable->name);
                    break;
                case qil::Opcode::LifetimeEnd:
                    backend.destroy(static_cast<typename Backend::LocalVariableData>(i.variable->data));
                    break;
                case qil::Opcode::PushString:
                    stack.push_back(backend.load(static_cast<typename Backend::StringLiteralData>(i.stringLiteral->data)));
                    break;
                case qil::Opcode::LoadVarPtr:
                    lValReg = backend.loadPtr(static_cast<typename Backend::LocalVariableData>(i.variable->data));
                    break;
                case qil::Opcode::PushVar:
                    stack.push_back(backend.load(static_cast<typename Backend::LocalVariableData>(i.variable->data)));
                    break;
                case qil::Opcode::Assign:
                    backend.swap(lValReg, stack.back());
                    lValReg = nullptr;
                    backend.destroy(stack.back());
                    stack.pop_back();
                    break;
                case qil::Opcode::Discard:
                    backend.destroy(stack.back());
                    stack.pop_back();
                    break;
                case qil::Opcode::LoadUnique:
                    stack.push_back(backend.loadUnique(lValReg));
                    break;
                case qil::Opcode::Trim:
                    backend.trim(stack.back());
                    break;
                case qil::Opcode::Add: {
                    typename Backend::Value r = stack.back();
                    stack.pop_back();
                    typename Backend::Value l = stack.back();
                    stack.back() = backend.add(l, r);
                    backend.destroy(r);
                    backend.destroy(l);
                    break;
                }
                case qil::Opcode::Print: {
                    typename Backend::Value v = stack.back();
                    stack.pop_back();
                    backend.print(v);
                    backend.destroy(v);
                    break;
                }
                default:
                    QORE_UNREACHABLE("Not implemented " << i);
            }
        }
    }

protected:
    Backend &backend;
};

//oddelit create local variable od lifetime start
template<typename Backend>
class Runner {

public:
    Runner(const analyzer::Script &script, Backend &backend) : script(script), backend(backend) {
        for (const auto &s : script.getStrings()) {
            s->data = backend.createStringLiteral(s->value);
        }
    }

    ~Runner() {
        for (const auto &s : script.getStrings()) {
            backend.destroy(static_cast<typename Backend::StringLiteralData>(s->data));
        }
    }

    void run() {
        RunnerBase<Backend> r(backend);
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
        for (const auto &v : variables) {
            backend.destroy(static_cast<typename Backend::LocalVariableData>(v->data));
        }
        for (const auto &s : strings) {
            backend.destroy(static_cast<typename Backend::StringLiteralData>(s->data));
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
    RunnerBase<Backend> rrr;
    std::vector<std::unique_ptr<qil::Variable>> variables;
    std::map<std::string, qil::Variable *> varLookup;
    std::vector<std::unique_ptr<qil::StringLiteral>> strings;
    std::map<std::string, qil::StringLiteral *> strLookup;
};

} // namespace qore

#endif // TOOLS_DRIVER_RUNNER_H_
