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
/// \brief Defines tags used by DumpVisitor for formatting the output.
///
/// These structures are used to distinguish between the overloads of the `<<` operator
/// implemented by formatters.
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_AST_DUMP_TAGS_H_
#define INCLUDE_QORE_AST_DUMP_TAGS_H_

namespace qore {
namespace ast {
namespace dump {

/**
 * \brief Indicates the start of an AST node.
 */
struct BeginNode {
    /**
     * \brief The type of the node.
     *
     * `nullptr` is used for virtual nodes such as the `operator` of a BinaryExpression.
     */
    const char *type;

    /**
     * \brief Constructor.
     * \param type the type of the node
     */
    BeginNode(const char *type = nullptr) : type(type) {
    }
};

/**
 * \brief An attribute of an AST node.
 * \tparam T the type of the attribute value
 */
template<typename T>
struct Attribute {
    const char *name;       //!< The name of the attribute.
    const T &value;         //!< The value of the attribute.

    /**
     * \brief Constructor.
     * \param name the name of the attribute
     * \param value the value of the attribute
     */
    Attribute(const char *name, const T &value) : name(name), value(value) {
    }
};

/**
 * \brief A source range attribute.
 */
struct Range : public Attribute<const SourceRange &> {
    /**
     * \brief Constructor.
     * \param value the value of the attribute
     */
    Range(const SourceRange &value) : Attribute("range", value) {
    }
};

/**
 * \brief Indicates the end of an AST node header, i.e. the end of node attributes.
 */
struct EndNodeHeader {};

/**
 * \brief Indicates the end of an AST node.
 */
struct EndNode {};

/**
 * \brief Indicates the start of an array of nodes.
 */
struct BeginArray {};

/**
 * \brief Indicates the end of an array of nodes.
 */
struct EndArray {};

/**
 * \brief Used immediately before BeginArray or BeginNode to give give a name to the child element.
 */
struct Child {
    const char *name;           //!< The name of the child element, `nullptr` is used before an element of an array.

    /**
     * \brief Constructor.
     * \param name the name of the child element
     */
    Child(const char *name = nullptr) : name(name) {
    }
};

/**
 * \brief Used immediately before Child or Attribute to indicate that the end of the node will follow.
 */
struct Last {};

} // namespace dump
} // namespace ast
} // namespace qore

#endif // INCLUDE_QORE_AST_DUMP_TAGS_H_
