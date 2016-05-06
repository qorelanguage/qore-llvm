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
#ifndef INCLUDE_QORE_COMP_SEM_CORE_H_
#define INCLUDE_QORE_COMP_SEM_CORE_H_

#include <map>
#include <string>
#include <utility>
#include <vector>
#include "qore/core/util/Util.h"
#include "qore/comp/Context.h"
#include "qore/comp/ast/Declaration.h"
#include "qore/comp/ast/Statement.h"
#include "qore/comp/ast/Type.h"
#include "qore/comp/sem/BlockScope.h"
#include "qore/comp/sem/expr/IntLiteralExpression.h"
#include "qore/comp/sem/expr/NothingLiteralExpression.h"
#include "qore/comp/sem/expr/StringLiteralRefExpression.h"

namespace qore {
namespace comp {
namespace sem {

class Builder;
class NamespaceScope;
class ClassScope;
class GlobalVariableInfo;
class FunctionOverloadPack;
class FunctionScope;
class FunctionBuilder;

class Core {

public:
    explicit Core(Context &ctx) : ctx(ctx) {
        builtinTypes[ctx.getStringTable().put("int")] = std::make_pair(&Type::Int, &Type::IntOpt);
        builtinTypes[ctx.getStringTable().put("string")] = std::make_pair(&Type::String, &Type::StringOpt);
        builtinTypes[ctx.getStringTable().put("any")] = std::make_pair(&Type::Any, nullptr);
    }

    void processPendingDeclarations();

    void addToQueue(ClassScope &c) {
        classQueue.push_back(&c);
    }

    void addToQueue(GlobalVariableInfo &gv) {
        globalVariableQueue.push_back(&gv);
    }

    void addToQueue(FunctionOverloadPack &fp) {
        functionOverloadPackQueue.push_back(&fp);
    }

    void addToQueue(FunctionScope &f) {
        functionQueue.emplace_back(&f);
    }

    /*
     * transforms ast to ir (it does type checking, resolves all symbols and operators, decides
     * which local variables need to be shared), adds closures to the queue (?)
     */
    Statement::Ptr doPass1(Scope &scope, ast::Statement &stmt);

    /*
     * transforms ir to as (exception safety, temporaries, ...)
     */
    void doPass2(Builder &builder, Statement &stmt);

    std::vector<Statement::Ptr> takeInitializers() {
        return std::move(initializers);
    }

    void addInitializer(Statement::Ptr stmt) {
        initializers.push_back(std::move(stmt));
    }

    Expression::Ptr defaultFor(const Type &type) {
        if (type == Type::String) {
            return StringLiteralRefExpression::create(createStringLiteral(""));
        }
        if (type == Type::Int) {
            return IntLiteralExpression::create(0);
        }
        if (type.acceptsNothing()) {
            return NothingLiteralExpression::create();
        }
        QORE_NOT_IMPLEMENTED("Default value");
    }

    qore::String *createStringLiteral(const std::string &value) {
        auto it = strings.find(value);
        if (it != strings.end()) {
            return it->second;
        }
        qore::String *str = ctx.getEnv().addString(value);
        strings.insert(std::make_pair(std::move(value), str));
        return str;
    }

    const Type *getBuiltinType(const ast::Name::Id &name, bool asterisk) const {
        auto it = builtinTypes.find(name.str);
        if (it == builtinTypes.end()) {
            return nullptr;
        }

        if (asterisk) {
            if (!it->second.second) {
                //FIXME report error (any with asterisk)
                return &Type::Error;
            }
            return it->second.second;
        }
        return it->second.first;
    }

private:
    Core(const Core &) = delete;
    Core(Core &&) = delete;
    Core &operator=(const Core &) = delete;
    Core &operator=(Core &&) = delete;

public:
    Context &ctx;

private:
    std::vector<ClassScope *> classQueue;
    std::vector<GlobalVariableInfo *> globalVariableQueue;
    std::vector<FunctionOverloadPack *> functionOverloadPackQueue;
    std::vector<FunctionScope *> functionQueue;
    std::vector<Statement::Ptr> initializers;
    std::unordered_map<std::string, qore::String *> strings;
    std::map<String::Ref, std::pair<const Type *, const Type *>> builtinTypes;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_CORE_H_
