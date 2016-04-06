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
/// \brief Defines type AST nodes.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_TYPE_H_
#define INCLUDE_QORE_COMP_AST_TYPE_H_

#include "qore/comp/Token.h"
#include "qore/comp/ast/Name.h"
#include "qore/comp/ast/Node.h"
#include "qore/comp/ast/Visitor.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Type representation in the AST.
 */
class Type {

public:
    /**
     * \brief Identifies the kind of the type.
     */
    enum class Kind {
        Basic,              //!< Identifies basic types represented by a name.
        Asterisk,           //!< Identifies types represented by a name with an asterisk.
        Implicit,           //!< Implicit type.
        Invalid,            //!< Invalid type.
    };

public:
    /**
     * \brief Creates a basic type.
     * \param name the name of the type
     * \return new \ref Kind::Basic type
     */
    static Type createBasic(Name name) {
        return Type(Kind::Basic, name.getStart(), std::move(name));
    }

    /**
     * \brief Creates an astrisk type.
     * \param asteriskLocation the location of the asterisk
     * \param name the name of the type
     * \return new \ref Kind::Asterisk type
     */
    static Type createAsterisk(SourceLocation asteriskLocation, Name name) {
        return Type(Kind::Asterisk, asteriskLocation, std::move(name));
    }

    /**
     * \brief Creates the implicit type.
     * \param location the implied location of the type
     * \return new \ref Kind::Implicit type
     */
    static Type createImplicit(SourceLocation location) {
        return Type(Kind::Implicit, location, Name::invalid(location));
    }

    /**
     * \brief Creates an invalid type.
     * TODO remove invalid type
     * \return new \ref Kind::Invalid type
     */
    static Type createInvalid() {
        return Type(Kind::Invalid, SourceLocation(), Name::invalid());
    }

    /**
     * \brief Default move-constructor.
     */
    Type(Type &&) = default;

    /**
     * \brief Default move-assignment.
     */
    Type &operator=(Type &&) = default;

    /**
     * \brief Returns the starting location of the type.
     * \return the starting location of the type
     */
    SourceLocation getStart() const {
        return start;
    }

    /**
     * \brief Returns the ending location of the type.
     * \return the ending location of the type
     */
    SourceLocation getEnd() const {
        return name.getEnd();
    }

    /**
     * \brief Returns the kind of the type.
     * \return the kind of the type
     */
    Kind getKind() const {
        return kind;
    }

    /**
     * \brief Returns the name of the type.
     *
     * This method can be called only for \ref Kind::Basic and \ref Kind::Asterisk types.
     * \return the name of the type
     */
    const Name &getName() const {
        assert(kind == Kind::Basic || kind == Kind::Asterisk);
        return name;
    }

private:
    Type(Kind kind, SourceLocation start, Name name) : kind(kind), start(start), name(std::move(name)) {
    }

    Type(const Type &) = delete;
    Type &operator=(const Type &) = delete;

private:
    Kind kind;
    SourceLocation start;
    Name name;
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_TYPE_H_
