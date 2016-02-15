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
#define NODE(name, body)    void visit(name &node) override { \
                                os << indent++ << "-" << #name << "@" << decode(node.getStart())  \
                                   << "-" << decode(node.getEnd()) << "\n"; \
                                body \
                                --indent; \
                            }
#define FIELD(name, sep)    os << indent << "." << name << ":" sep
#define ARRAY(name, body)   FIELD(#name, "\n"); ++indent; for (auto &i : node.name) { body } --indent
#define NODE_ARRAY(name)    ARRAY(name, i->accept(*this);)
#define VISIT(name)         FIELD(#name, "\n"); ++indent; node.name->accept(*this); --indent
#define MODIFIERS(name)     FIELD(#name, ""); doModifiers(node.name); os << "\n"
#define TOKEN(name)         FIELD(#name, " "); doToken(node.name)
#define NAME(name)          FIELD(#name, " "); doName(node.name); os << "\n"
#define VISIT_DIRECT(name)  FIELD(#name, "\n"); ++indent; visit(*node.name); --indent
#define BOOL(name)          FIELD(#name, " ") << (node.name ? "true" : "false") << "\n"

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

    NODE(Class, {
            MODIFIERS(modifiers);
            NAME(name);
            ARRAY(inherits, os << indent << "-"; doName(i.second); doModifiers(i.first, false); os << "\n"; );
            NODE_ARRAY(members);
    })

    NODE(NamespaceConstant, {
            visit(*node.constant);
    })

    NODE(GlobalVariable, {
            MODIFIERS(modifiers);
            VISIT(type);
            NAME(name);
    })

    NODE(Function, {
            visit(*node.routine);
    })

    NODE(Method, {
            visit(*node.routine);
    })

    NODE(ClassConstant, {
            visit(*node.constant);
    })

    NODE(MemberGroup, {
            MODIFIERS(modifiers);
            NODE_ARRAY(members);
    })

    NODE(Field, {
            MODIFIERS(modifiers);
            VISIT(type);
            TOKEN(name);
            if (node.exprInit) {
                VISIT(exprInit);
            }
            if (node.argListInit) {
                VISIT_DIRECT(argListInit);
            }
    })

    NODE(EmptyStatement, {
    })

    NODE(ExpressionStatement, {
            VISIT(expression);
    })

    NODE(CompoundStatement, {
            NODE_ARRAY(statements);
    })

    NODE(ReturnStatement, {
            if (node.expression) {
                VISIT(expression);
            }
    })

    NODE(IfStatement, {
            VISIT(condition);
            VISIT(stmtTrue);
            if (node.stmtFalse) {
                VISIT(stmtFalse);
            }
    })

    NODE(TryStatement, {
            VISIT(stmtTry);
            VISIT(exceptionType);
            TOKEN(exceptionName);
            VISIT(stmtCatch);
    })

    NODE(ForeachStatement, {
            VISIT(varType);
            TOKEN(varName);
            VISIT(expr);
            VISIT(stmt);
    })

    NODE(ThrowStatement, {
            NODE_ARRAY(exprList);
    })

    NODE(SimpleStatement, {
            TOKEN(keyword);
    })

    NODE(ScopeGuardStatement, {
            TOKEN(keyword);
            VISIT(stmt);
    })

    NODE(WhileStatement, {
            VISIT(expr);
            VISIT(stmt);
    })

    NODE(DoWhileStatement, {
            VISIT(stmt);
            VISIT(expr);
    })

    NODE(ForStatement, {
            if (node.init) {
                VISIT(init);
            }
            if (node.condition) {
                VISIT(condition);
            }
            if (node.update) {
                VISIT(update);
            }
            VISIT(stmt);
    })

    NODE(SwitchStatement, {
            VISIT(expr);
            ARRAY(body,
                    os << indent++ << "-" << lexeme(i->keyword);
                    if (i->op.type != TokenType::None) {
                        os << " " << lexeme(i->op);
                    }
                    os << ":\n";
                    if (i->expr) {
                        FIELD("expr", "\n");
                        ++indent;
                        i->expr->accept(*this);
                        --indent;
                    }
                    if (!i->statements.empty()) {
                        FIELD("statements", "\n");
                        ++indent;
                        for (auto &s : i->statements) {
                            s->accept(*this);
                        }
                        --indent;
                    }
                    --indent;
            );
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
            NODE_ARRAY(data);
    })

    NODE(HashExpression, {
            ARRAY(data, i.first->accept(*this); i.second->accept(*this););
    })

    NODE(VarDeclExpression, {
            VISIT(type);
            TOKEN(name);
    })

    NODE(CastExpression, {
            VISIT(type);
            VISIT(expression);
    })

    NODE(CallExpression, {
            VISIT(calee);
            VISIT_DIRECT(argList);
    })

    NODE(UnaryExpression, {
            VISIT(expr);
            TOKEN(op);
            BOOL(postfix);
    })

    NODE(IndexExpression, {
            VISIT(left);
            VISIT(right);
            TOKEN(endToken);
    })

    NODE(AccessExpression, {
            VISIT(expr);
            TOKEN(token);
    })

    NODE(NewExpression, {
            NAME(name);
            VISIT_DIRECT(argList);
    })

    NODE(BinaryExpression, {
            VISIT(left);
            TOKEN(op);
            VISIT(right);
    })

    NODE(InstanceofExpression, {
            VISIT(expr);
            NAME(name);
    })

    NODE(ConditionalExpression, {
            VISIT(condition);
            VISIT(exprTrue);
            VISIT(exprFalse);
    })

    NODE(AssignmentExpression, {
            VISIT(left);
            TOKEN(op);
            VISIT(right);
    })

    NODE(ListOperationExpression, {
            TOKEN(op);
            NODE_ARRAY(args);
    })

    NODE(ClosureExpression, {
            visit(*node.routine);
    })

    NODE(NameType, {
            NAME(name);
    })

    NODE(AsteriskType, {
            NAME(name);
    })

    NODE(ImplicitType, {
    })

    void visit(Constant &node) {
        MODIFIERS(modifiers);
        NAME(name);
        VISIT(initializer);
    }

    void visit(Routine &node) {
        MODIFIERS(modifiers);
        VISIT(type);
        NAME(name);
        ARRAY(params,
                std::get<0>(i)->accept(*this);
                os << indent << "-"; doToken(std::get<1>(i));
                if (std::get<2>(i)) std::get<2>(i)->accept(*this); else os << indent << "-no default-\n";
        );
        if (!node.baseCtors.empty()) {
            ARRAY(baseCtors, os << indent++ << "-"; doName(i.first); os << "\n"; visit(*i.second); --indent; );
        }
        if (node.body) {
            VISIT(body);
        }
    }

    void visit(ArgList &node) {
        for (auto &i : node.data) {
            i->accept(*this);
        }
    }

private:
    std::string decode(const SourceLocation &location) {
        assert(location.sourceId >= 0);
        std::pair<int, int> l = srcMgr.get(location.sourceId).decodeLocation(location.offset);
        std::ostringstream str;
        str << l.first << ":" << l.second;
        return str.str();
    }

    void doModifiers(const Modifiers &mods, bool printEmpty = true) {
        if (mods.isEmpty() && printEmpty) os << " -none-";
        if (mods.contains(Modifier::Abstract)) os << " abstract";
        if (mods.contains(Modifier::Deprecated)) os << " deprecated";
        if (mods.contains(Modifier::Final)) os << " final";
        if (mods.contains(Modifier::Private)) os << " private";
        if (mods.contains(Modifier::Public)) os << " public";
        if (mods.contains(Modifier::Static)) os << " static";
        if (mods.contains(Modifier::Synchronized)) os << " synchronized";
    }

    void doToken(const Token &token) {
        os << token.type;
        if (token.type != TokenType::None) {
            os << " " << lexeme(token);
        }
        os << "\n";
    }

    void doName(const Name &name) {
        if (name.tokens.empty()) {
            os << "-none-";
        } else {
            auto it = name.tokens.begin();
            os << lexeme(*it++);
            while (it != name.tokens.end()) {
                os << "::" << lexeme(*it++);
            }
        }
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
#undef FIELD
#undef ARRAY
#undef NODE_ARRAY
#undef VISIT
#undef MODIFIERS
#undef TOKEN
#undef NAME
#undef VISIT_DIRECT
#undef BOOL

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
