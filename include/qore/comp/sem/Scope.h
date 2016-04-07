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
#ifndef INCLUDE_QORE_COMP_SEM_SCOPE_H_
#define INCLUDE_QORE_COMP_SEM_SCOPE_H_

#include <string>
#include "qore/common/Exceptions.h"
#include "qore/ir/Type.h"
#include "qore/ir/decl/Function.h"
#include "qore/ir/decl/StringLiteral.h"
#include "qore/ir/expr/Expression.h"
#include "qore/ir/expr/IntLiteralExpression.h"
#include "qore/ir/expr/NothingLiteralExpression.h"
#include "qore/ir/expr/StringLiteralRefExpression.h"
#include "qore/comp/ast/Type.h"
#include "qore/comp/ast/Name.h"
#include "qore/comp/sem/Symbol.h"
#include "qore/rt/Types.h"

namespace qore {
namespace comp {
namespace sem {

class Scope {

public:
    virtual ~Scope() = default;
    virtual const ir::Type &resolveType(ast::Type &node) = 0;
    virtual Symbol resolve(ast::Name &name) = 0;
    virtual const ir::StringLiteral &createStringLiteral(const std::string &value) = 0;

    ir::Expression::Ptr defaultFor(const ir::Type &type) {
        if (type == ir::Types::String) {
            return ir::StringLiteralRefExpression::create(createStringLiteral(""));
        }
        if (type == ir::Types::Int) {
            return ir::IntLiteralExpression::create(0);
        }
        if (type == ir::Types::Any) {
            return ir::NothingLiteralExpression::create();
        }
        QORE_NOT_IMPLEMENTED("Default value");
    }

    const ir::Function &resolveOperator(rt::Op o, const ir::Type &l, const ir::Type &r) {
        if (l.getKind() == ir::Type::Kind::Builtin && r.getKind() == ir::Type::Kind::Builtin) {
            //FIXME catch exception end report diagnostic
            //FIXME this switch should be replaced by a direct lookup
            switch (rt::meta::findOperator(o, static_cast<const ir::BuiltinType &>(l).getRuntimeType(),
                    static_cast<const ir::BuiltinType &>(r).getRuntimeType())) {
                case rt::Operator::IntPlusInt:
                    return ir::Functions::IntPlusInt;
                case rt::Operator::StringPlusString:
                    return ir::Functions::StringPlusString;
                case rt::Operator::AnyPlusAny:
                    return ir::Functions::AnyPlusAny;
                case rt::Operator::AnyPlusEqAny:
                    return ir::Functions::AnyPlusEqAny;
                default:
                    QORE_NOT_IMPLEMENTED("");
            }
        }
        QORE_NOT_IMPLEMENTED("");
    }

    const ir::Function *resolveConversion(const ir::Type &src, const ir::Type &dst) {
        if (src.getKind() == ir::Type::Kind::Builtin && dst.getKind() == ir::Type::Kind::Builtin) {
            switch (rt::meta::findConversion(static_cast<const ir::BuiltinType &>(src).getRuntimeType(),
                    static_cast<const ir::BuiltinType &>(dst).getRuntimeType())) {
                case rt::Conversion::Identity:
                    return nullptr;
                case rt::Conversion::IntToString:
                    return &ir::Functions::IntToString;
                case rt::Conversion::StringToInt:
                    return &ir::Functions::StringToInt;
                case rt::Conversion::BoxInt:
                    return &ir::Functions::BoxInt;
                default:
                    QORE_NOT_IMPLEMENTED("");
            }
        }
        QORE_NOT_IMPLEMENTED("");
    }
};

class BlockScope : public Scope {
public:
    virtual Symbol declareLocalVariable(String::Ref name, const ir::Type &type) = 0;
};

class BlockScopeImpl : public BlockScope {

public:
    BlockScopeImpl(Context &ctx, Scope &parent, ir::UserFunction &f) : ctx(ctx), parent(parent), f(f) {
    }

    const ir::Type &resolveType(ast::Type &node) override {
        return parent.resolveType(node);
    }

    Symbol declareLocalVariable(String::Ref name, const ir::Type &type) override {
        std::string n = ctx.getStringTable().get(name);
        Symbol s = Symbol(f.createLocalVariable(n, type));
        symbols[n] = s;
        return s;
    }

    Symbol resolve(ast::Name &name) override {
        if (name.isSimple()) {
            const std::string &n = ctx.getStringTable().get(name.last().str);
            auto it = symbols.find(n);
            if (it != symbols.end()) {
                return it->second;
            }
        }
        return parent.resolve(name);
    }

    const ir::StringLiteral &createStringLiteral(const std::string &value) override {
        return parent.createStringLiteral(value);
    }

private:
    Context &ctx;
    Scope &parent;
    ir::UserFunction &f;
    std::unordered_map<std::string, Symbol> symbols;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_SCOPE_H_
