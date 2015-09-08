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
/// \brief Helper for indenting output.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMMON_INDENT_H_
#define INCLUDE_QORE_COMMON_INDENT_H_

#include <ostream>
#include <string>

namespace qore {

/**
 * \brief Helper for producing indentation.
 *
 * Keeps track of current indentation level, which can be changed using the increment/decrement operators
 * (both prefix and postfix). The resulting indentation (two spaces per level) can be retrieved using the
 * get() method or the whole object can be sent to an output stream directly using
 * the \ref operator<<(std::ostream &os, const Indent &indent) "<< operator".
 *
 * For example:
 *
 *     Indent indent;
 *     std::cout << "A\n" << ++indent << "B\n" << indent-- << "C\n" << indent << "D\n";
 *
 * Produces:
 *
 *     A
 *       B
 *       C
 *     D
 */
class Indent {

public:
    /**
     * \brief The indentation size. i.e. the number of spaces per level.
     */
    static constexpr int Multiplier = 2;

public:
    /**
     * \brief Constructor.
     * \param level initial indentation level (can be negative)
     */
    Indent(int level = 0) : level(level) {
    }

    /**
     * \brief Increases the indentation level, prefix version.
     * \return `this`
     */
    Indent &operator++() {
        ++level;
        return *this;
    }

    /**
     * \brief Increases the indentation level, postfix version.
     * \return `this` before the change
     */
    Indent operator++(int) {
        Indent i = *this;
        ++*this;
        return i;
    }

    /**
     * \brief Decreases the indentation level, prefix version.
     * \return `this`
     */
    Indent &operator--() {
        --level;
        return *this;
    }

    /**
     * \brief Decreases the indentation level, postfix version.
     * \return `this` before the change
     */
    Indent operator--(int) {
        Indent i = *this;
        --*this;
        return i;
    }

    /**
     * \brief Returns the indentation string.
     * \return the indentation
     */
    std::string get() const {
        return std::string(level * Multiplier, ' ');
    }

private:
    int level;
};

/**
 * \brief Writes the indentation to an output stream.
 * \param os the output stream
 * \param indent the indentation
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const Indent &indent) {
    return os << indent.get();
}

} // namespace qore

#endif // INCLUDE_QORE_COMMON_INDENT_H_
