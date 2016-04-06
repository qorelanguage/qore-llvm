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
/// \brief Repository of all known types.
///
//------------------------------------------------------------------------------
#include "qore/comp/semantic/TypeRegistry.h"
#include <string>
#include "qore/comp/semantic/Class.h"
#include "ReportedError.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Represents a built-in type.
 */
class BuiltinType : public Type {

public:
    /**
     * \brief Creates a type with given name.
     * \param ctx the compiler context
     * \param name the name of the type
     */
    BuiltinType(Context &ctx, String::Ref name) : ctx(ctx), name(name) {
    }

    std::string getName() const override {
        return ctx.getStringTable().get(name);
    }

private:
    Context &ctx;
    String::Ref name;
};

/**
 * \brief Represents the implicit type.
 */
class ImplicitType : public Type {

public:
    /**
     * \brief Constructor.
     */
    ImplicitType() = default;

    std::string getName() const override {
        return "<implicit>";
    }
};

/**
 * \brief Represents the error type.
 */
class ErrorType : public Type {

public:
    /**
     * \brief Constructor.
     */
    ErrorType() = default;

    std::string getName() const override {
        return "<error>";
    }
};

/**
 * \brief Represents a type defined by a class.
 */
class ClassType : public Type {

public:
    /**
     * \brief Creates the type for given class.
     * \param clazz the class
     */
    explicit ClassType(Class &clazz) : clazz(clazz) {
    }

    std::string getName() const override {
        return clazz.getFullName();
    }

private:
    Class &clazz;
};

/**
 * \brief Represents the type *T for a type T.
 */
class AsteriskType : public Type {

public:
    /**
     * \brief Creates the type *T for given type T.
     * \param type the original type
     */
    explicit AsteriskType(const Type &type) : type(type) {
    }

    std::string getName() const override {
        return  "*" + type.getName();
    }

private:
    const Type &type;
};

TypeRegistry::TypeRegistry(Context &ctx)
        : ctx(ctx), implicit(std::make_shared<ImplicitType>()), error(std::make_shared<ErrorType>()) {
    makeBuiltin("int");
    makeBuiltin("string");
}

void TypeRegistry::makeBuiltin(std::string name) {
    LOG("Creating " << name);
    String::Ref str = ctx.getStringTable().put(name);
    builtinTypes[str] = std::make_shared<BuiltinType>(ctx, str);
}

Type::Ref TypeRegistry::resolve(const Scope &scope, const ast::Type &node) {
    if (node.getKind() == ast::Type::Kind::Implicit) {
        return Type::Ref(implicit);
    }
    if (node.getKind() == ast::Type::Kind::Invalid || !node.getName().isValid()) {
        return Type::Ref(error);
    }
    std::shared_ptr<Type> type;
    if (node.getName().isSimple()) {
        auto it = builtinTypes.find(node.getName().last().str);
        if (it != builtinTypes.end()) {
            type = it->second;
        }
    }
    if (!type) {
        try {
            Class &c = scope.resolveClass(node.getName());
            std::shared_ptr<Type> &p = classTypes[&c];
            if (!p) {
                p = std::make_shared<ClassType>(c);
            }
            type = p;
        } catch (ReportedError &) {
            return Type::Ref(error);
        }
    }
    if (node.getKind() == ast::Type::Kind::Basic) {
        return Type::Ref(type);
    }
    assert(node.getKind() == ast::Type::Kind::Asterisk);
    //TODO handle *any, *nothing etc
    std::shared_ptr<Type> &p = asteriskTypes[type.get()];
    if (!p) {
        p = std::make_shared<AsteriskType>(*type);
    }
    return Type::Ref(p);
}

} // namespace semantic
} // namespace comp
} // namespace qore
