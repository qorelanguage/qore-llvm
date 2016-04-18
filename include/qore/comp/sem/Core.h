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
#include "qore/common/Util.h"
#include "qore/comp/Context.h"
#include "qore/comp/ast/Declaration.h"
#include "qore/comp/ast/Statement.h"
#include "qore/comp/ast/Type.h"
#include "qore/comp/as/GlobalVariable.h"
#include "qore/comp/as/Function.h"
#include "qore/comp/as/Script.h"
#include "qore/comp/as/StringLiteral.h"
#include "qore/comp/sem/BlockScope.h"
#include "qore/comp/sem/stmt/GlobalVariableInitializationStatement.h"
#include "qore/comp/sem/stmt/StringLiteralInitializationStatement.h"
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

class ScriptBuilder {

public:
    explicit ScriptBuilder(StringTable &strings) {
        builtinTypes[strings.put("int")] = std::make_pair(&as::Type::Int, &as::Type::IntOpt);
        builtinTypes[strings.put("string")] = std::make_pair(&as::Type::String, &as::Type::StringOpt);
        builtinTypes[strings.put("any")] = std::make_pair(&as::Type::Any, nullptr);
    }

    const as::Type *getBuiltinType(const ast::Name::Id &name, bool asterisk) const {
        auto it = builtinTypes.find(name.str);
        if (it == builtinTypes.end()) {
            return nullptr;
        }

        if (asterisk) {
            if (!it->second.second) {
                //FIXME report error (any with asterisk)
                return &as::Type::Error;
            }
            return it->second.second;
        }
        return it->second.first;
    }

    const as::Type &getClassType(const ClassScope &c, bool asterisk);

    as::StringLiteral createStringLiteral(const std::string &value) {
        auto it = strings.find(value);
        if (it != strings.end()) {
            return it->second;
        }
        as::StringLiteral sl(strings.size());
        strings.insert(std::make_pair(value, sl));
        initializers.push_back(StringLiteralInitializationStatement::create(sl, std::move(value)));
        return sl;
    }

    as::GlobalVariable &createGlobalVariable(String::Ref name, SourceLocation location, const as::Type &type) {
        std::unique_ptr<as::GlobalVariable> ptr
            = util::make_unique<as::GlobalVariable>(globalVariables.size(), name, location, type);
        as::GlobalVariable &gv = *ptr;
        globalVariables.push_back(std::move(ptr));
        initializers.push_back(GlobalVariableInitializationStatement::create(gv, defaultFor(type)));
        return gv;
    }

    Expression::Ptr defaultFor(const as::Type &type) {
        if (type == as::Type::String) {
            return StringLiteralRefExpression::create(createStringLiteral(""));
        }
        if (type == as::Type::Int) {
            return IntLiteralExpression::create(0);
        }
        if (type == as::Type::Any || type == as::Type::Error || type.isOptional()) {
            return NothingLiteralExpression::create();
        }
        QORE_NOT_IMPLEMENTED("Default value");
    }

    as::Function &createFunction(std::string name, Id tempCount, Id localCount, std::vector<as::Block::Ptr> blocks) {
        std::unique_ptr<as::Function> ptr
            = util::make_unique<as::Function>(tempCount, localCount, std::move(blocks));
        as::Function &f = *ptr;
        functions[name] = std::move(ptr);
        return f;
    }

    as::Script::Ptr build() {
        return util::make_unique<as::Script>(std::move(types), std::move(globalVariables), std::move(functions));
    }

    std::vector<Statement::Ptr> takeInitializers() {
        return std::move(initializers);
    }

private:
    const as::Type &createType(rt::Type runtimeType, std::string name, bool optional) {
        std::unique_ptr<as::Type> ptr = util::make_unique<as::Type>(runtimeType, std::move(name), optional, false);
        as::Type &t = *ptr;
        types.push_back(std::move(ptr));
        return t;
    }

private:
    std::map<String::Ref, std::pair<const as::Type *, const as::Type *>> builtinTypes;
    std::map<const ClassScope *, std::pair<const as::Type *, const as::Type *>> classTypes;
    std::unordered_map<std::string, as::StringLiteral> strings;
    std::vector<Statement::Ptr> initializers;
    std::vector<std::unique_ptr<as::GlobalVariable>> globalVariables;
    std::vector<std::unique_ptr<as::Type>> types;
    std::map<std::string, std::unique_ptr<as::Function>> functions;
};


class Core {

public:
    explicit Core(Context &ctx) : ctx(ctx), scriptBuilder(ctx.getStringTable()) {
    }

    virtual ~Core() = default;

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

    void addToQueue(BlockScope &f, ast::Statement &stmt) {
        functionQueue.emplace_back(&f, &stmt);
    }

    /*
     * transforms ast to ir (it does type checking, resolves all symbols and operators, decides
     * which local variables need to be shared), adds closures to the queue (?)
     */
    Statement::Ptr doPass1(BlockScope &scope, ast::Statement &stmt);

    /*
     * transforms ir to as (exception safety, temporaries, ...)
     */
    void doPass2(Builder &builder, Statement &stmt);

private:
    Core(const Core &) = delete;
    Core(Core &&) = delete;
    Core &operator=(const Core &) = delete;
    Core &operator=(Core &&) = delete;

public:
    Context &ctx;
    ScriptBuilder scriptBuilder;

private:
    std::vector<ClassScope *> classQueue;
    std::vector<GlobalVariableInfo *> globalVariableQueue;
    std::vector<FunctionOverloadPack *> functionOverloadPackQueue;
    std::vector<std::pair<BlockScope *, ast::Statement *>> functionQueue;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_CORE_H_
