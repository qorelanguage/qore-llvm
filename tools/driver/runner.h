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

template<typename Backend, typename Storage>
class RunnerBase {

public:
    RunnerBase(Backend &backend = Backend(), Storage &storage = Storage()) : backend(backend), storage(storage) {
    }

    virtual ~RunnerBase() {
    }

    void run(const analyzer::Code &code) {
        std::deque<typename Backend::Value> stack;
        typename Backend::LValue lValReg = nullptr;
        bool done = false;
        for (const auto &i : code) {
            std::cout << "LVALREG: " << lValReg << "\n";
            std::cout << "Stack:\n";
            for (const auto &v : stack) {
                std::cout << "  " << v << "\n";
            }
            std::cout << "------" << i << "\n";
            switch (i.getOpcode()) {
                case analyzer::Opcode::LifetimeStart:
                    storage.variableData[i.getLocVar()->getId()] = backend.createLocalVariable(i.getLocVar()->getName());
                    break;
                case analyzer::Opcode::LifetimeEnd:
                    backend.destroy(storage.variableData[i.getLocVar()->getId()]);
                    break;
                case analyzer::Opcode::PushString:
                    stack.push_back(backend.load(storage.stringData[i.getStrLit()->getId()]));
                    break;
                case analyzer::Opcode::LoadLocVarPtr:
                    lValReg = backend.loadPtr(&storage.variableData[i.getLocVar()->getId()]);
                    break;
                case analyzer::Opcode::PushLocVar:
                    stack.push_back(backend.load(storage.variableData[i.getLocVar()->getId()]));
                    break;
                case analyzer::Opcode::Swap:
                    backend.swap(lValReg, stack.back());
                    break;
                case analyzer::Opcode::CleanupLValue:
                    lValReg = nullptr;
                    break;
                case analyzer::Opcode::PopAndDeref:
                    backend.destroy(stack.back());
                    stack.pop_back();
                    break;
                case analyzer::Opcode::LoadUnique:
                    stack.push_back(backend.loadUnique(lValReg));
                    break;
                case analyzer::Opcode::Trim:
                    backend.trim(stack.back());
                    break;
                case analyzer::Opcode::Add: {
                    typename Backend::Value r = stack.back();
                    stack.pop_back();
                    typename Backend::Value l = stack.back();
                    stack.back() = backend.add(l, r);
                    backend.destroy(r);
                    backend.destroy(l);
                    break;
                }
                case analyzer::Opcode::Print: {
                    typename Backend::Value v = stack.back();
                    stack.pop_back();
                    backend.print(v);
                    backend.destroy(v);
                    break;
                }
                case analyzer::Opcode::Ret:
                    backend.ret();
                    done = true;
                    break;
                default:
                    QORE_UNREACHABLE("Not implemented " << i);
            }
            if (done) {
                break;
            }
        }
    }

protected:
    Backend &backend;
    Storage &storage;
};

//oddelit create local variable od lifetime start
template<typename Backend>
class Runner {

public:
    Runner(const analyzer::Script &script, Backend &backend) : script(script), backend(backend), stringData(script.getStrings().size()), variableData(script.getVariables().size()) {
        for (const auto &s : script.getStrings()) {
            stringData[s->getId()] = backend.createStringLiteral(s->getValue());
        }
    }

    ~Runner() {
        for (auto &sd : stringData) {
            backend.destroy(sd);
        }
    }

    void run() {
        RunnerBase<Backend, Runner> r(backend, *this);
        r.run(script.getCode());
    }

private:
    const analyzer::Script &script;
    Backend &backend;
    std::vector<typename Backend::StringLiteralData> stringData;
    std::vector<typename Backend::LocalVariableData> variableData;

    friend class RunnerBase<Backend, Runner>;
};


template<typename Backend>
class IntAnalyzer : private analyzer::Scope {

public:
    IntAnalyzer(Backend backend = Backend()) : backend(backend), rrr(backend, *this) {
    }

    ~IntAnalyzer() {
        for (auto &vd : variableData) {
            backend.destroy(vd.second);
        }
        for (auto &sd : stringData) {
            backend.destroy(sd.second);
        }
    }

    void analyze(const ast::Statement::Ptr &node) {
        analyzer::script::Visitor<> visitor(*this, code);
        node->accept(visitor);
        //TODO if error reported, clear code
        //TODO interactive diag manager -> throw exception, catch it here
        rrr.run(code);
        code = analyzer::Code();
    }

    analyzer::LocalVariable *createLocalVariable(const std::string &name, const SourceRange &range) override {
        analyzer::LocalVariable *&var = varLookup[name];
        if (var) {
            //TODO error redeclaration
        } else {
            var = create(name, range);
            code.add(analyzer::Ins(analyzer::Opcode::LifetimeStart, var));
        }
        return var;
    }

    analyzer::LocalVariable *resolve(const std::string &name, const SourceRange &range) override {
        analyzer::LocalVariable *&var = varLookup[name];
        if (!var) {
            //TODO ERROR: undeclared
        }
        return var;
    }

    analyzer::StringLiteral *createStringLiteral(const std::string &value, const SourceRange &range) override {
        analyzer::StringLiteral *&s = strLookup[value];
        if (!s) {
            s = new analyzer::StringLiteral(strings.size(), value, range);
            strings.emplace_back(s);
            stringData[s->getId()] = backend.createStringLiteral(value);
        }
        return s;
    }

private:
    analyzer::LocalVariable *create(const std::string &name, const SourceRange &range) {
        analyzer::LocalVariable *v = new analyzer::LocalVariable(variables.size(), name, range);
        variables.emplace_back(v);
        return v;
    }

private:
    Backend backend;
    analyzer::Code code;
    RunnerBase<Backend, IntAnalyzer> rrr;
    std::vector<std::unique_ptr<analyzer::LocalVariable>> variables;
    std::map<std::string, analyzer::LocalVariable *> varLookup;
    std::vector<std::unique_ptr<analyzer::StringLiteral>> strings;
    std::map<std::string, analyzer::StringLiteral *> strLookup;
    std::map<int, typename Backend::LocalVariableData> variableData;
    std::map<int, typename Backend::StringLiteralData> stringData;

    friend class RunnerBase<Backend, IntAnalyzer>;
};

} // namespace qore

#endif // TOOLS_DRIVER_RUNNER_H_
