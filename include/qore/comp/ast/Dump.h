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
/// \brief Utility class for dumping Abstract Syntax Trees.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_DUMP_H_
#define INCLUDE_QORE_COMP_AST_DUMP_H_

#include "qore/comp/ast/Visitor.h"
#include "qore/comp/ast/Script.h"

namespace qore {
namespace comp {
namespace ast {

/// \cond IGNORED_BY_DOXYGEN
#define NODE(name, body)    void visit(ast::name &node) override { Node n_(*this, #name); body }
#define ARRAY(name)         { Array a_(*this, #name); for (auto &i : node.name) { i->accept(*this); } }
#define LOCATION(name)      doLocation(#name, node.name)
#define MODIFIERS(name)     doModifiers(#name, node.name)
#define TOKEN(name)         doToken(#name, node.name)

template<typename OS>
class DumpVisitor : public ast::DeclarationVisitor {

public:
    DumpVisitor(SourceManager &srcMgr, OS &os) : srcMgr(srcMgr), os(os) {
    }

    NODE(Script, {
            ARRAY(members);
    })

    NODE(Namespace, {
            LOCATION(location);
            MODIFIERS(modifiers);
            TOKEN(name);
            ARRAY(members);
    })

private:
    struct Node {
        Node(DumpVisitor &dv, const std::string &name) : dv(dv) {
            dv.os << dv.indent++ << "-" << name << "\n";
        }
        ~Node() {
            --dv.indent;
        }
        DumpVisitor &dv;
    };

    struct Array {
        Array(DumpVisitor &dv, const std::string &name) : dv(dv) {
            dv.os << dv.indent++ << "." << name << ":\n";
        }
        ~Array() {
            --dv.indent;
        }
        DumpVisitor &dv;
    };

    void doLocation(const std::string &name, const SourceLocation &location) {
        std::pair<int, int> l = srcMgr.get(location.sourceId).decodeLocation(location.offset);
        os << indent << "." << name << ": " << l.first << ":" << l.second << "\n";
    }

    void doModifiers(const std::string &name, const ast::Modifiers &mods) {
        os << indent << "." << name << ":";
        if (mods.isEmpty()) os << " -none-";
        if (mods.contains(ast::Modifier::Abstract)) os << " abstract";
        if (mods.contains(ast::Modifier::Deprecated)) os << " deprecated";
        if (mods.contains(ast::Modifier::Final)) os << " final";
        if (mods.contains(ast::Modifier::Private)) os << " private";
        if (mods.contains(ast::Modifier::Public)) os << " public";
        if (mods.contains(ast::Modifier::Static)) os << " static";
        if (mods.contains(ast::Modifier::Synchronized)) os << " synchronized";
        os << "\n";
    }

    void doToken(const std::string &name, const Token &token) {
        std::string lexeme = srcMgr.get(token.location.sourceId).getRange(token.location.offset, token.length);
        os << indent << "." << name << ": " << token.type << " " << lexeme << "\n";
    }

private:
    SourceManager &srcMgr;
    OS &os;
    qore::log::Indent indent;
};
#undef NODE
#undef ARRAY
#undef LOCATION
#undef MODIFIERS
#undef TOKEN

template<typename OS, typename N>
void dump(SourceManager &srcMgr, OS &os, N &n) {
    DumpVisitor<OS> dumpVisitor(srcMgr, os);
    dumpVisitor.visit(n);
}
/// \endcond

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_DUMP_H_
