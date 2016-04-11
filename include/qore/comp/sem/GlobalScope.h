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
#ifndef INCLUDE_QORE_COMP_SEM_GLOBALSCOPE_H_
#define INCLUDE_QORE_COMP_SEM_GLOBALSCOPE_H_

#include <string>
#include <unordered_map>
#include "qore/ir/decl/Script.h"
#include "qore/comp/sem/Scope.h"

namespace qore {
namespace comp {
namespace sem {

class GlobalScope : public Scope {

public:
    GlobalScope(Context &ctx, ir::Script &script) : ctx(ctx), script(script) {
    }

    const ir::Type &resolveType(ast::Type &node) override {
        if (node.getKind() == ast::Type::Kind::Basic && node.getName().isSimple()) {
            const std::string &n = ctx.getStringTable().get(node.getName().last().str);
            if (n == "string") {
                return ir::Types::String;
            }
            if (n == "int") {
                return ir::Types::Int;
            }
            if (n == "any") {
                return ir::Types::Any;
            }
        }
        QORE_NOT_IMPLEMENTED("resolve type");
    }

    Symbol resolve(ast::Name &name) override {
        if (name.isSimple()) {
            const std::string &n = ctx.getStringTable().get(name.last().str);
            auto it = symbols.find(n);
            if (it != symbols.end()) {
                return it->second;
            }
        }
        QORE_NOT_IMPLEMENTED("");
    }

    const ir::StringLiteral &createStringLiteral(const std::string &value) override {
        auto &x = strings[value];
        if (!x) {
            x = &script.createStringLiteral(value);
        }
        return *x;
    }

    virtual Symbol declareGlobalVariable(ast::Name &name, const ir::Type &type) {
        if (name.isSimple()) {
            const std::string &n = ctx.getStringTable().get(name.last().str);
            const ir::GlobalVariable &gv = script.createGlobalVariable(n, type);
            addGlobalVariableInitializer(gv, defaultFor(type));
            Symbol s = Symbol(gv);
            symbols[n] = s;
            return s;
        }
        QORE_NOT_IMPLEMENTED("");
    }

protected:
    virtual void addGlobalVariableInitializer(const ir::GlobalVariable &gv, ir::Expression::Ptr init) = 0;

private:
    Context &ctx;
    ir::Script &script;
    std::unordered_map<std::string, Symbol> symbols;
    std::unordered_map<std::string, const ir::StringLiteral *> strings;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_GLOBALSCOPE_H_
