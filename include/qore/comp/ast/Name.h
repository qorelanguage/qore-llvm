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
#include "qore/comp/Token.h"

namespace qore {
namespace comp {
namespace ast {

/**
 * \brief Describes a name, which is a nonempty sequence of identifiers.
 */
class Name {

public:
    std::vector<Token> tokens;                              //!< The identifiers of the name.

    Name() = default;

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
        assert(!tokens.empty());
        return tokens[0].location;
    }

    /**
     * \brief Returns the end location of the name the source code.
     * \return the end location
     */
    SourceLocation getEnd() const {
        assert(!tokens.empty());
        return tokens[tokens.size() - 1].location;
    }

private:
    Name(const Name &) = delete;
    Name &operator=(const Name &) = delete;
};

} // namespace ast
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AST_NAME_H_
