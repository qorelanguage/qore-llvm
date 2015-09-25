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
/// \brief A visitor for dumping the AST.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_DUMPVISITOR_H_
#define INCLUDE_QORE_AST_DUMPVISITOR_H_

#include "qore/ast/Program.h"
#include "qore/ast/Visitor.h"
#include "qore/ast/dump/Tags.h"

namespace qore {
namespace ast {

/**
 * \brief Contains classes for visualising the Abstract Syntax Tree.
 */
namespace dump {

/**
 * \private
 */
template<typename F>
class FormatterWrapper {

public:
    FormatterWrapper(F &formatter, bool ignoreNext) : formatter(formatter), ignoreNext(ignoreNext) {
    }

    template<typename T>
    F &operator <<(Attribute<T> attribute) {
        if (!ignoreNext) {
            formatter << attribute;
        }
        return formatter;
    }

    F &operator <<(Child child) {
        if (!ignoreNext) {
            formatter << child;
        }
        return formatter;
    }

private:
    F &formatter;
    bool ignoreNext;
};

/**
 * \private
 */
struct Unless {
    bool value;

    Unless(bool value) : value(value) {
    }
};

/**
 * \private
 */
template<typename F>
inline FormatterWrapper<F> operator <<(F &f, Unless u) {
    return FormatterWrapper<F>(f, u.value);
}

/**
 * \brief A visitor for dumping the AST.
 * \tparam F the type of the formatter, see XmlFormat, JsonFormat, YamlFormat and CompactFormat
 */
template<typename F>
class DumpVisitor : public ExpressionVisitor, public StatementVisitor, public ProgramVisitor {

public:
    /**
     * \brief Creates the visitor with given formatter.
     * \param formatter the formatter for producing the output
     */
    DumpVisitor(F formatter = F()) : formatter(formatter) {
    }

    void visit(const IntegerLiteral *node) override {
        formatter << BeginNode("integerLiteral")
            << Range(node->getRange())
            << Last() << attribute("value", node->value)
            << EndNode();
    }

    void visit(const StringLiteral *node) override {
        formatter << BeginNode("stringLiteral")
            << Range(node->getRange())
            << Last() << attribute("value", node->value)
            << EndNode();
    }

    void visit(const BinaryExpression *node) override {
        formatter << BeginNode("binaryExpression")
            << Range(node->getRange())
            << EndNodeHeader()
            << Child("left"), visitNode(node->left)
            << Child("operator") << BeginNode()
                << Last() << Range(node->operatorRange)
                << EndNode()
            << Last() << Child("right"), visitNode(node->right)
            << EndNode();
    }

    void visit(const UnaryExpression *node) override {
        formatter << BeginNode("unaryExpression")
            << Range(node->getRange())
            << EndNodeHeader()
            << Child("operator") << BeginNode()
                << Last() << Range(node->operatorRange)
                << EndNode()
            << Last() << Child("operand"), visitNode(node->operand)
            << EndNode();
    }

    void visit(const Assignment *node) override {
        formatter << BeginNode("assignment")
            << Range(node->getRange())
            << EndNodeHeader()
            << Child("left"), visitNode(node->left)
            << Child("operator") << BeginNode()
                << Last() << Range(node->operatorRange)
                << EndNode()
            << Last() << Child("right"), visitNode(node->right)
            << EndNode();
    }

    void visit(const VarDecl *node) override {
        formatter << BeginNode("varDecl")
            << Range(node->getRange())
            << Last() << attribute("name", node->name)
            << EndNode();
    }

    void visit(const Identifier *node) override {
        formatter << BeginNode("identifier")
            << Range(node->getRange())
            << Last() << attribute("name", node->name)
            << EndNode();
    }

    void visit(const EmptyStatement *node) override {
        formatter << BeginNode("emptyStatement")
            << Last() << Range(node->getRange())
            << EndNode();
    }

    void visit(const PrintStatement *node) override {
        formatter << BeginNode("printStatement")
            << Range(node->getRange())
            << EndNodeHeader()
            << Last() << Child("expression"), visitNode(node->expression)
            << EndNode();
    }

    void visit(const ExpressionStatement *node) override {
        formatter << BeginNode("expressionStatement")
            << Range(node->getRange())
            << EndNodeHeader()
            << Last() << Child("expression"), visitNode(node->expression)
            << EndNode();
    }

    void visit(const CompoundStatement *node) override {
        formatter << BeginNode("compoundStatement")
            << Range(node->getRange())
            << EndNodeHeader()
            << Last() << Child("statements"), visitNodes(node->statements)
            << EndNode();
    }

    void visit(const IfStatement *node) override {
        formatter << BeginNode("ifStatement")
            << Range(node->getRange())
            << EndNodeHeader()
            << Child("condition"), visitNode(node->condition)
            << Child("then"), visitNode(node->thenBranch)
            << Last() << Child("else"), visitNode(node->elseBranch)
            << EndNode();
    }

    void visit(const TryStatement *node) override {
        formatter << BeginNode("tryStatement")
            << Range(node->getRange())
            << EndNodeHeader()
            << Child("try"), visitNode(node->tryBody)
            << Unless(node->var == nullptr) << Child("var"), visitNode(node->var)
            << Last() << Child("catch"), visitNode(node->catchBody)
            << EndNode();
    }

    void visit(const Program *node) override {
        formatter << BeginNode("program")
            << Range(node->getRange())
            << EndNodeHeader()
            << Last() << Child("body"), visitNodes(node->body)
            << EndNode();
    }

private:
    template<typename N>
    F &visitNode(const N &node) {
        if (node) {
            node->accept(*this);
        }
        return formatter;
    }

    template<typename N>
    F &visitNodes(const N &nodes) {
        formatter << BeginArray();
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (it == nodes.end() - 1) {
                formatter << Last();
            }
            formatter << Child();
            (*it)->accept(*this);
        }
        return formatter << EndArray();
    }

    template<typename T>
    static Attribute<T> attribute(const char *name, const T value) {
        return Attribute<T>(name, value);
    }

private:
    F formatter;
};

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMPVISITOR_H_
