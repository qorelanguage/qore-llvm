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
/// \brief Implements the NamespaceScope class.
///
//------------------------------------------------------------------------------
#include "qore/comp/sem/NamespaceScope.h"
#include <vector>
#include "qore/core/util/Util.h"
#include "ReportedError.h"

namespace qore {
namespace comp {
namespace sem {

void NamespaceScope::processDeclaration(ast::Declaration &decl) {
    try {
        switch (decl.getKind()) {
            case ast::Declaration::Kind::Namespace: {
                ast::Namespace &namespaceDecl = static_cast<ast::Namespace &>(decl);
                NamespaceScope &parent = findParentFor(namespaceDecl.name);
                parent.processNamespaceDeclaration(namespaceDecl);
                break;
            }
            case ast::Declaration::Kind::Class: {
                ast::Class &classDecl = static_cast<ast::Class &>(decl);
                NamespaceScope &parent = findParentFor(classDecl.name);
                parent.processClassDeclaration(classDecl);
                break;
            }
            case ast::Declaration::Kind::Constant:
                //add to const-queue
                QORE_NOT_IMPLEMENTED("");
            case ast::Declaration::Kind::Function: {
                ast::Function &functionDecl = static_cast<ast::Function &>(decl);
                NamespaceScope &parent = findParentFor(functionDecl.name);
                parent.processFunctionDeclaration(functionDecl);
                break;
            }
            case ast::Declaration::Kind::GlobalVariable: {
                ast::GlobalVariable &globalVariableDecl = static_cast<ast::GlobalVariable &>(decl);
                NamespaceScope &parent = findParentFor(globalVariableDecl.name);
                parent.processGlobalVariableDeclaration(globalVariableDecl);
                break;
            }
            default:
                QORE_UNREACHABLE("Invalid ast::Declaration::Kind");
        }
    } catch (ReportedError &) {
        // ignored, diagnostic has been reported already
    }
}

void NamespaceScope::processNamespaceDeclaration(ast::Namespace &node) {
    String::Ref name = node.name.last().str;
    SourceLocation location = node.name.last().location;

    //TODO check modifiers, reserved words
    NamespaceScope *ns = findNamespace(name);
    if (!ns) {
        if (ClassScope *c = findClass(name)) {
            core.report(DiagId::SemaDuplicateNamespaceName, location) << name << getNameForDiag();
            core.report(DiagId::SemaPreviousDeclaration, c->getRt().getLocation());
            throw ReportedError();
        }

        Namespace &rtNs = rt.addNamespace(core.getString(name), location);
        Ptr ptr = Ptr(new NamespaceScope(rtNs, *this));
        ns = ptr.get();
        namespaces[name] = std::move(ptr);
    } else {
        //TODO check that modifiers are consistent with previous declaration
    }

    for (auto &decl : node.members) {
        ns->processDeclaration(*decl);
    }
}

void NamespaceScope::processClassDeclaration(ast::Class &node) {
    String::Ref name = node.name.last().str;
    SourceLocation location = node.name.last().location;

    //TODO check modifiers, reserved words
    ClassScope *old = findClass(name);
    NamespaceScope *ns = findNamespace(name);
    if (old || ns) {
        core.report(DiagId::SemaDuplicateClassName, location) << name << getNameForDiag();
        core.report(DiagId::SemaPreviousDeclaration, old ? old->getRt().getLocation() : ns->rt.getLocation());
        throw ReportedError();
    }

    Class &c = rt.addClass(core.getString(name), location); //XXX when should this be created?
    ClassScope::Ptr ptr = ClassScope::Ptr(new ClassScope(c, core, *this, node));
    core.addToQueue(*ptr);
    classes[name] = std::move(ptr);
}

void NamespaceScope::processGlobalVariableDeclaration(ast::GlobalVariable &node) {
    String::Ref name = node.name.last().str;
    SourceLocation location = node.name.last().location;

    //TODO check modifiers, reserved words
    GlobalVariableInfo *old = findGlobalVariable(name);
    FunctionGroupInfo *fg = findFunctionGroup(name);
    //FIXME constant
    if (old || fg) {
        core.report(DiagId::SemaDuplicateGlobalVariableName, location) << name << getNameForDiag();
        core.report(DiagId::SemaPreviousDeclaration, old ? old->getLocation() : fg->getLocation());
        throw ReportedError();
    }

    GlobalVariableInfo::Ptr ptr = GlobalVariableInfo::Ptr(new GlobalVariableInfo(core, *this, node));
    core.addToQueue(*ptr);
    globalVariables[name] = std::move(ptr);
}

void NamespaceScope::processFunctionDeclaration(ast::Function &node) {
    String::Ref name = node.name.last().str;
    SourceLocation location = node.name.last().location;

    FunctionGroupInfo *fg = findFunctionGroup(name);
    if (!fg) {
        GlobalVariableInfo *gv = findGlobalVariable(name);
        //FIXME constant
        if (gv) {
            core.report(DiagId::SemaDuplicateFunctionName, location) << name << getNameForDiag();
            core.report(DiagId::SemaPreviousDeclaration, gv->getLocation());
            throw ReportedError();
        }

        FunctionGroupInfo::Ptr ptr = FunctionGroupInfo::Ptr(
                new FunctionGroupInfo(rt.addFunctionGroup(core.getString(name)), core, *this, location));
        fg = ptr.get();
        functions[name] = std::move(ptr);
    }

    fg->addOverload(*node.routine, location);
}

ClassScope &NamespaceScope::resolveClass(const ast::Name &name) const {
    assert(name.isValid());
    if (name.isRoot()) {
        if (ClassScope *c = getRoot().lookupClass(name.begin(), name.end())) {
            return *c;
        }
        core.report(DiagId::SemaUnresolvedClass, name.getStart()) << name;
        throw ReportedError();
    }
    if (parentNamespace) {
        if (ClassScope *c = lookupClass(name.begin(), name.end())) {
            return *c;
        }
        return parentNamespace->resolveClass(name);
    }
    std::vector<ClassScope *> classes;
    collectClasses(classes, name.begin(), name.end());
    if (classes.size() == 1) {
        return *classes[0];
    }
    core.report(classes.empty() ? DiagId::SemaUnresolvedClass : DiagId::SemaAmbiguousClass, name.getStart()) << name;
    throw ReportedError();
}

ClassScope *NamespaceScope::lookupClass(ast::Name::Iterator begin, ast::Name::Iterator end) const {
    assert(begin != end);

    if (begin + 1 == end) {
        return findClass(begin->str);
    }
    if (NamespaceScope *ns = findNamespace(begin->str)) {
        return ns->lookupClass(begin + 1, end);
    }
    return nullptr;
}

void NamespaceScope::collectClasses(std::vector<ClassScope *> &classes, ast::Name::Iterator begin,
        ast::Name::Iterator end) const {
    assert(begin != end);

    if (begin + 1 == end) {
        if (ClassScope *c = findClass(begin->str)) {
            classes.push_back(c);
        }
    } else if (NamespaceScope *ns = findNamespace(begin->str)) {
        if (ClassScope *c = ns->lookupClass(begin + 1, end)) {
            classes.push_back(c);
        }
    }
    for (auto &it : namespaces) {
        it.second->collectClasses(classes, begin, end);
    }
}

NamespaceScope &NamespaceScope::findParentFor(const ast::Name &name) {
    if (!name.isValid()) {
        throw ReportedError();
    }
    if (name.isRoot()) {
        core.report(DiagId::SemaInvalidNamespaceMemberName, name.getStart());
        throw ReportedError();
    }
    NamespaceScope *parent = this;
    for (ast::Name::Iterator it = name.begin(); it != name.end() - 1; ++it) {
        if (NamespaceScope *ns = parent->findNamespace(it->str)) {
            parent = ns;
        } else {
            core.report(DiagId::SemaNamespaceNotFound, it->location) << it->str << parent->getNameForDiag();
            throw ReportedError();
        }
    }
    return *parent;
}

Symbol NamespaceScope::resolveSymbol(const ast::Name &name) const {
    if (name.isSimple()) {
        auto it = globalVariables.find(name.last().str);
        if (it != globalVariables.end()) {
            return Symbol(*it->second);
        }
        auto it2 = functions.find(name.last().str);
        if (it2 != functions.end()) {
            return Symbol(*it2->second);
        }
    }
    QORE_NOT_IMPLEMENTED("");
}

const Type &NamespaceScope::resolveType(const ast::Type &node) const {
    if (node.getKind() == ast::Type::Kind::Implicit) {
        return Type::Nothing;
    }
    if (node.getKind() == ast::Type::Kind::Invalid || !node.getName().isValid()) {
        return Type::Error;
    }

    assert(node.getKind() == ast::Type::Kind::Basic || node.getKind() == ast::Type::Kind::Asterisk);
    bool asterisk = node.getKind() == ast::Type::Kind::Asterisk;

    if (node.getName().isSimple()) {
        const Type *t = core.getBuiltinType(node.getName().last(), asterisk);
        if (t) {
            return *t;
        }
    }

    try {
        ClassScope &c = resolveClass(node.getName());
        return c.getRt().getType(asterisk);
    } catch (ReportedError &) {
        return Type::Error;
    }
}

} // namespace sem
} // namespace comp
} // namespace qore
