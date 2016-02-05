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
/// \brief Representation of modifiers such as public or static.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AST_MODIFIERS_H_
#define INCLUDE_QORE_COMP_AST_MODIFIERS_H_

#include <type_traits>
#include "qore/common/Exceptions.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Enumeration of all modifiers.
 */
enum class Modifier {
    Abstract        = 1 << 0,   //!< The `abstract` modifier.
    Deprecated      = 1 << 1,   //!< The `deprecated` modifier.
    Final           = 1 << 2,   //!< The `final` modifier.
    Private         = 1 << 3,   //!< The `private` modifier.
    Public          = 1 << 4,   //!< The `public` modifier.
    Static          = 1 << 5,   //!< The `static` modifier.
    Synchronized    = 1 << 6,   //!< The `synchronized` modifier.
};

/**
 * \brief Writes modifier name to an output stream.
 * \param o the output stream
 * \param m the modifier to write
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &o, Modifier m) {
    switch (m) {
        case Modifier::Abstract:        return o << "abstract";
        case Modifier::Deprecated:      return o << "deprecated";
        case Modifier::Final:           return o << "final";
        case Modifier::Private:         return o << "private";
        case Modifier::Public:          return o << "public";
        case Modifier::Static:          return o << "static";
        case Modifier::Synchronized:    return o << "synchronized";
    }
    QORE_UNREACHABLE("Invalid value of DiagLevel: " << static_cast<int>(m));
}

/**
 * \brief A set of modifiers.
 */
class Modifiers {

public:
    /**
     * \brief Constructs an empty set.
     */
    constexpr Modifiers() : value(0) {
    }

    /**
     * \brief Constructs a new set containing a single modifier.
     * \param m the initial member of the set
     */
    // cppcheck-suppress noExplicitConstructor
    constexpr Modifiers(Modifier m) : value(static_cast<int>(m)) {
    }

    /**
     * \brief Determines whether the set is empty.
     * \return `true` if the set is empty
     */
    constexpr bool isEmpty() const {
        return value == 0;
    }

    /**
     * \brief Determines whether the set contains given modifier.
     * \param mod the modifier to check
     * \return true if `mod` is a member of this set
     */
    constexpr bool contains(Modifier mod) const {
        return (value & static_cast<int>(mod)) != 0;
    }

    /**
     * \brief Determines whether this set is a subset of another set.
     * \param superSet the superset to check
     * \return true if this set is a subset of `superSet`
     */
    constexpr bool isSubsetOf(const Modifiers &superSet) const {
        return (value & ~superSet.value) == 0;
    }

    /**
     * \brief Creates a union of two sets.
     * \param other the other set
     * \return a new set containing everything from this set and the `other` set
     */
    constexpr Modifiers operator|(Modifiers other) const {
        return Modifiers(value | other.value);
    }

    /**
     * \brief Adds all member from another set into this set.
     * \param other the other set
     * \return `this` for chaining
     */
    Modifiers operator|=(Modifiers other) {
        value |= other.value;
        return *this;
    }

private:
    int value;

    explicit constexpr Modifiers(int v) : value(v) {
    }
};

/**
 * \brief Combines two modifiers together.
 * \param m1 the first modifier
 * \param m2 the second modifier
 * \return a set of modifiers containing m1 and m2
 */
constexpr Modifiers operator|(Modifier m1, Modifier m2) {
    return Modifiers(m1) | m2;
}

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_MODIFIERS_H_
