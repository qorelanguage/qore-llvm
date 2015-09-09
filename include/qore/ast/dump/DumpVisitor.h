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
 * \brief A visitor for dumping the AST.
 * \tparam F the type of the formatter, see XmlFormat, JsonFormat, YamlFormat and CompactFormat
 */
template<typename F>
class DumpVisitor : public Visitor {

public:
    /**
     * \brief Creates the visitor with given formatter.
     * \param formatter the formatter for producing the output
     */
    DumpVisitor(F formatter = F()) : formatter(formatter) {
    }

    void* visit(const IntegerLiteral *node) override {
        formatter << BeginNode("integerLiteral")
            << Range(node->getRange())
            << Last() << attribute("value", node->value)
            << EndNode();
        return nullptr;
    }

    void* visit(const StringLiteral *node) override {
        formatter << BeginNode("stringLiteral")
            << Range(node->getRange())
            << Last() << attribute("value", node->value)
            << EndNode();
        return nullptr;
    }

    void* visit(const BinaryExpression *node) override {
        formatter << BeginNode("binaryExpression")
            << Range(node->getRange())
            << EndNodeHeader()
            << Child("left"), visitNode(node->left)
            << Child("operator") << BeginNode()
                << Last() << Range(node->operatorRange)
                << EndNode()
            << Last() << Child("right"), visitNode(node->right)
            << EndNode();
        return nullptr;
    }

    void* visit(const EmptyStatement *node) override {
        formatter << BeginNode("emptyStatement")
            << Last() << Range(node->getRange())
            << EndNode();
        return nullptr;
    }

    void* visit(const PrintStatement *node) override {
        formatter << BeginNode("printStatement")
            << Range(node->getRange())
            << EndNodeHeader()
            << Last() << Child("expression"), visitNode(node->expression)
            << EndNode();
        return nullptr;
    }

    void* visit(const Program *node) override {
        formatter << BeginNode("program")
            << Range(node->getRange())
            << EndNodeHeader()
            << Last() << Child("body"), visitNodes(node->body)
            << EndNode();
        return nullptr;
    }

private:
    template<typename N>
    F &visitNode(const N &node) {
        node->accept(*this);
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
