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

#include <string>
#include <utility>
#include "qore/comp/Context.h"
#include "qore/comp/ast/Visitor.h"
#include "qore/comp/ast/Script.h"

namespace qore {
namespace comp {
namespace ast {

/// \cond IGNORED_BY_DOXYGEN
#define NODE_HEADER(name)   os << indent++ << "-" << #name << "@" << decode(node.getStart())  \
                               << "-" << decode(node.getEnd()) << "\n"
#define NODE_FOOTER()       --indent
#define NODE(name, body)    void visit(name &node) { \
                                NODE_HEADER(name); \
                                body \
                                NODE_FOOTER(); \
                            }
#define FIELD(name, sep)    os << indent << "." << name << ":" sep
#define ARRAY(name, body)   FIELD(#name, "\n"); ++indent; for (auto &i : node.name) { body } --indent
#define NODE_ARRAY(name)    ARRAY(name, i->accept(*this);)
#define VISIT(name)         FIELD(#name, "\n"); ++indent; node.name->accept(*this); --indent
#define TYPE(name)          FIELD(#name, "\n"); ++indent; visit(node.name); --indent
#define MODIFIERS(name)     FIELD(#name, ""); doModifiers(node.name); os << "\n"
#define TOKEN(name)         FIELD(#name, " "); doToken(node.name)
#define STR(name)           FIELD(#name, " ") << str(node.name) << "\n"
#define NAME(name)          FIELD(#name, " "); doName(node.name); os << "\n"
#define VISIT_DIRECT(name)  FIELD(#name, "\n"); ++indent; visit(*node.name); --indent
#define BOOL(name)          FIELD(#name, " ") << (node.name ? "true" : "false") << "\n"

template<typename OS>
class DumpVisitor : public StatementVisitor, public ExpressionVisitor {

public:
    DumpVisitor(Context &ctx, OS &os) : ctx(ctx), os(os) {
    }

    void visit(Script &node) {
        NODE_HEADER(Script);
        ARRAY(members, visit(*i););
        NODE_ARRAY(statements);
        NODE_FOOTER();
    }

    void visit(Declaration &decl) {
        switch (decl.getKind()) {
            case Declaration::Kind::Namespace:
                visit(static_cast<Namespace &>(decl));
                break;
            case Declaration::Kind::GlobalVariable:
                visit(static_cast<GlobalVariable &>(decl));
                break;
            case Declaration::Kind::Function:
                visit(static_cast<Function &>(decl));
                break;
            case Declaration::Kind::Constant:
                visit(static_cast<Constant &>(decl));
                break;
            case Declaration::Kind::Class:
                visit(static_cast<Class &>(decl));
                break;
            case Declaration::Kind::Method:
                visit(static_cast<Method &>(decl));
                break;
            case Declaration::Kind::Field:
                visit(static_cast<Field &>(decl));
                break;
            case Declaration::Kind::MemberGroup:
                visit(static_cast<MemberGroup &>(decl));
                break;
        }
    }

    NODE(Namespace, {
            MODIFIERS(modifiers);
            NAME(name);
            ARRAY(members, visit(*i););
    })

    NODE(Class, {
            MODIFIERS(modifiers);
            NAME(name);
            ARRAY(inherits, os << indent << "-"; doName(i.second); doModifiers(i.first, false); os << "\n";);
            ARRAY(members, visit(*i););
    })

    NODE(GlobalVariable, {
            MODIFIERS(modifiers);
            TYPE(type);
            NAME(name);
    })

    NODE(Function, {
            NAME(name);
            visit(*node.routine);
    })

    NODE(Method, {
            NAME(name);
            visit(*node.routine);
    })

    NODE(MemberGroup, {
            MODIFIERS(modifiers);
            ARRAY(members, visit(*i););
    })

    NODE(Field, {
            MODIFIERS(modifiers);
            TYPE(type);
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
            TYPE(exceptionType);
            TOKEN(exceptionName);
            VISIT(stmtCatch);
    })

    NODE(ForeachStatement, {
            TYPE(varType);
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
            ARRAY(body, {
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
            });
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
            TYPE(type);
            STR(name);
    })

    NODE(CastExpression, {
            TYPE(type);
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

    NODE(RegexExpression, {
            VISIT(left);
            TOKEN(op);
            TOKEN(regex);
    })

    NODE(ClosureExpression, {
            visit(*node.routine);
    })

    NODE(Constant, {
        MODIFIERS(modifiers);
        NAME(name);
        VISIT(initializer);
    })

    void visit(Type &node) {
        if (node.getKind() == Type::Kind::Implicit) {
            NODE_HEADER(ImplicitType);
        } else {
            if (node.getKind() == Type::Kind::Basic) {
                NODE_HEADER(NameType);
            } else {
                NODE_HEADER(AsteriskType);
            }
            FIELD("name", " "); doName(node.getName()); os << "\n";
        }
        NODE_FOOTER();
    }

    void visit(Routine &node) {
        MODIFIERS(modifiers);
        TYPE(type);
        ARRAY(params, {
                visit(std::get<0>(i));
                os << indent << "-"; doToken(std::get<1>(i));
                if (std::get<2>(i)) { std::get<2>(i)->accept(*this); } else { os << indent << "-no default-\n"; }
        });
        if (!node.baseCtors.empty()) {
            ARRAY(baseCtors, os << indent++ << "-"; doName(i.first); os << "\n"; visit(*i.second); --indent;);
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
        std::pair<int, int> l = ctx.getSrcMgr().get(location.sourceId).decodeLocation(location.offset);
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
        if (!name.isValid()) {
            os << "-invalid-";
            return;
        }
        auto it = name.begin();
        if (!name.isRoot()) {
            os << str((it++)->str);
        }
        while (it != name.end()) {
            os << "::" << str((it++)->str);
        }
    }

    std::string lexeme(const Token &token) {
        return ctx.getSrcMgr().get(token.location.sourceId).getRange(token.location.offset, token.length);
    }

    const std::string &str(String::Ref s) {
        return ctx.getStringTable().get(s);
    }

private:
    Context &ctx;
    OS &os;
    qore::log::Indent indent;
};

#undef NODE_HEADER
#undef NODE_FOOTER
#undef NODE
#undef FIELD
#undef ARRAY
#undef NODE_ARRAY
#undef VISIT
#undef TYPE
#undef MODIFIERS
#undef TOKEN
#undef NAME
#undef VISIT_DIRECT
#undef BOOL

template<typename OS, typename N>
void dump(Context &ctx, OS &os, N &n) {
    DumpVisitor<OS> dumpVisitor(ctx, os);
    dumpVisitor.visit(n);
}
/// \endcond

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_DUMP_H_
