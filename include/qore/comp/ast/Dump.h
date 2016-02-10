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
#define NODE(name, body)    void visit(name &node) override { NodeHelper n_(*this, #name, node); body }
#define ARRAY(name, body)   { ArrayHelper a_(*this, #name); for (auto &i : node.name) { body } }
#define NODE_ARRAY(name)    ARRAY(name, i->accept(*this);)
#define MODIFIERS(name)     doModifiers(#name, node.name)
#define TOKEN(name)         doToken(#name, node.name)
#define NAME(name)          doName(#name, node.name)
#define VISIT(name)         os << indent++ << "." << #name << ":\n"; node.name->accept(*this); --indent

template<typename OS>
class DumpVisitor : public DeclarationVisitor, public StatementVisitor, public ExpressionVisitor, public TypeVisitor {

public:
    DumpVisitor(SourceManager &srcMgr, OS &os) : srcMgr(srcMgr), os(os) {
    }

    NODE(Script, {
            NODE_ARRAY(members);
            NODE_ARRAY(statements);
    })

    NODE(Namespace, {
            MODIFIERS(modifiers);
            TOKEN(name);
            NODE_ARRAY(members);
    })

    NODE(EmptyStatement, {
    })

    NODE(ExpressionStatement, {
            VISIT(expression);
    })

    NODE(ErrorExpression, {
            TOKEN(token);
    })

    NODE(LiteralExpression, {
            TOKEN(token);
    })

    NODE(NameExpression, {
            NAME(name);
    })

    NODE(ListExpression, {
            TOKEN(startToken);
            NODE_ARRAY(data);
            TOKEN(endToken);
    })

    NODE(HashExpression, {
            TOKEN(startToken);
            ARRAY(data, i.first->accept(*this); i.second->accept(*this););
            TOKEN(endToken);
    })

    NODE(VarDeclExpression, {
            VISIT(type);
            TOKEN(name);
    })

    NODE(NameType, {
            NAME(name);
    })

    NODE(AsteriskType, {
            NAME(name);
    })

    NODE(ImplicitType, {
    })

private:
    struct NodeHelper {
        NodeHelper(DumpVisitor &dv, const std::string &name, Node &node) : dv(dv) {
            dv.os << dv.indent++ << "-" << name << "@" << dv.decode(node.getStart())
                    << "-" << dv.decode(node.getEnd()) << "\n";
        }
        ~NodeHelper() {
            --dv.indent;
        }
        DumpVisitor &dv;
    };

    struct ArrayHelper {
        ArrayHelper(DumpVisitor &dv, const std::string &name) : dv(dv) {
            dv.os << dv.indent++ << "." << name << ":\n";
        }
        ~ArrayHelper() {
            --dv.indent;
        }
        DumpVisitor &dv;
    };

    std::string decode(const SourceLocation &location) {
        assert(location.sourceId >= 0);
        std::pair<int, int> l = srcMgr.get(location.sourceId).decodeLocation(location.offset);
        std::ostringstream str;
        str << l.first << ":" << l.second;
        return str.str();
    }

    void doModifiers(const std::string &name, const Modifiers &mods) {
        os << indent << "." << name << ":";
        if (mods.isEmpty()) os << " -none-";
        if (mods.contains(Modifier::Abstract)) os << " abstract";
        if (mods.contains(Modifier::Deprecated)) os << " deprecated";
        if (mods.contains(Modifier::Final)) os << " final";
        if (mods.contains(Modifier::Private)) os << " private";
        if (mods.contains(Modifier::Public)) os << " public";
        if (mods.contains(Modifier::Static)) os << " static";
        if (mods.contains(Modifier::Synchronized)) os << " synchronized";
        os << "\n";
    }

    void doToken(const std::string &name, const Token &token) {
        os << indent << "." << name << ": " << token.type;
        if (token.type != TokenType::None) {
            os << " " << lexeme(token);
        }
        os << "\n";
    }

    void doName(const std::string &fieldName, const Name &name) {
        os << indent << "." << fieldName << ": ";
        bool first = true;
        for (const Token &token : name.tokens) {
            if (first) {
                first = false;
            } else {
                os << "::";
            }
            os << lexeme(token);
        }
        os << "\n";
    }

    std::string lexeme(const Token &token) {
        return srcMgr.get(token.location.sourceId).getRange(token.location.offset, token.length);
    }

private:
    SourceManager &srcMgr;
    OS &os;
    qore::log::Indent indent;
};
#undef NODE
#undef ARRAY
#undef MODIFIERS
#undef TOKEN
#undef VISIT
#undef NAME

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
