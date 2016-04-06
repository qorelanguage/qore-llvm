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
/// \brief Defines the Name structure.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_NAME_H_
#define INCLUDE_QORE_COMP_AST_NAME_H_

#include <cassert>
#include <utility>
#include <vector>
#include "qore/comp/DiagManager.h"
#include "qore/comp/SourceLocation.h"
#include "qore/comp/String.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Describes a name.
 *
 * A valid name is a nonempty sequence of identifiers separated by double colons, possibly starting with a double colon.
 * An instance of this class can also represent an invalid name which is used for error recovery.
 */
class Name {

public:
    /**
     * \brief An element of the name - identifier and its location.
     */
    struct Id {
        String::Ref str;            //!< The identifier.
        SourceLocation location;    //!< The location.

        /**
         * \brief Constructor.
         * \param str the indentifier
         * \param location the location
         */
        Id(String::Ref str, SourceLocation location) : str(str), location(location) {
        }
    };
    using Iterator = std::vector<Id>::const_iterator;       //!< Iterator type.

public:
    /**
     * \brief Creates a valid name.
     * \param start the starting location
     * \param root true if the name starts with a double colon
     * \param names the identifiers of the name
     */
    Name(SourceLocation start, bool root, std::vector<Id> names)
            : start(start), root(root), names(std::move(names)) {
        assert(isValid());
    }

    //TODO make location mandatory
    /**
     * \brief Creates an invalid name indicating an error in the source script.
     * \param location the location
     * \return an invalid name
     */
    static Name invalid(SourceLocation location = SourceLocation()) {
        return Name(location);
    }

    /**
     * \brief Default move-constructor.
     */
    Name(Name &&) = default;

    /**
     * \brief Default move-assignment.
     */
    Name &operator=(Name &&) = default;

    /**
     * \brief Returns the start location of the name in the source code.
     * \return the start location
     */
    SourceLocation getStart() const {
        return start;
    }

    /**
     * \brief Returns the end location of the name the source code.
     * \return the end location
     */
    SourceLocation getEnd() const {
        return isValid() ? last().location : start;
    }

    /**
     * \brief Returns true if the name is valid.
     * \return true if the name is valid
     */
    bool isValid() const {
        return !names.empty();
    }

    /**
     * \brief Returns true if the name starts with a double colon.
     *
     * It is illegal to call this method on an invalid name.
     * \return true if the name starts with a double colon
     */
    bool isRoot() const {
        assert(isValid());
        return root;
    }

    /**
     * \brief Returns an iterator to the first identifier of the name.
     *
     * It is illegal to call this method on an invalid name.
     * \return returns an iterator to the first identifier of the name
     */
    Iterator begin() const {
        assert(isValid());
        return names.begin();
    }

    /**
     * \brief Returns an iterator to the element following the last identifier of the name.
     *
     * It is illegal to call this method on an invalid name.
     * \return returns an iterator to the element following the last identifier of the name
     */
    Iterator end() const {
        assert(isValid());
        return names.end();
    }

    /**
     * \brief Returns the last token in the name.
     *
     * It is illegal to call this method on an invalid name.
     * \return the last identifier in the name
     */
    const Id &last() const {
        assert(isValid());
        return names[names.size() - 1];
    }

    /**
     * \brief Returns true if the name is a single identifier with no double colons.
     *
     * It is illegal to call this method on an invalid name.
     * \return true if the name is a single identifier with no double colons.
     */
    bool isSimple() const {
        assert(isValid());
        return !root && names.size() == 1;
    }

private:
    explicit Name(SourceLocation location) : start(location), root(false) {
        assert(!isValid());
    }

    Name(const Name &) = delete;
    Name &operator=(const Name &) = delete;

private:
    SourceLocation start;
    bool root;
    std::vector<Id> names;
};

} // namespace ast

/**
 * \brief Sets the value of a parameter of the message.
 *
 * The value will replace the next occurrence of '%s' in the message.
 * \param b the diagnostic message builder
 * \param name the value of the parameter, must be a valid name
 */
template<>
inline void toDiagArg(DiagBuilder &b, const ast::Name &name) {
    assert(name.isValid());
    std::ostringstream str;

    auto it = name.begin();
    if (!name.isRoot()) {
        str << b.getStringTable().get((it++)->str);
    }
    while (it != name.end()) {
        str << "::" << b.getStringTable().get((it++)->str);
    }
    b << str.str();
}

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_NAME_H_
