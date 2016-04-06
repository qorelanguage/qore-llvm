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
/// \brief Interface for symbols.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_SYMBOL_H_
#define INCLUDE_QORE_COMP_SEMANTIC_SYMBOL_H_

#include <string>
#include "qore/comp/SourceLocation.h"
#include "qore/comp/String.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Base class for all symbols.
 */
class Symbol {

public:
    /**
     * \brief Identifies the type of the symbol.
     */
    enum class Kind {
        Namespace,                  //!< Identifies instances of \ref Namespace.
        Class,                      //!< Identifies instances of \ref Class.
        GlobalVariable,             //!< Identifies instances of \ref GlobalVariable.
    };

public:
    using Ptr = std::unique_ptr<Symbol>;            //!< Unique pointer type.

public:
    virtual ~Symbol() = default;

    /**
     * \brief Returns the type of the symbol.
     * \return the type of the symbol
     */
    virtual Kind getKind() const = 0;

    /**
     * \brief Returns the name of the symbol.
     * \return the name of the symbol
     */
    virtual String::Ref getName() const = 0;

    /**
     * \brief Returns the location of the declaration.
     * \return the location of the declaration.
     */
    virtual SourceLocation getLocation() const = 0;

protected:
    Symbol() = default;

private:
    Symbol(const Symbol &) = delete;
    Symbol(Symbol &&) = delete;
    Symbol &operator=(const Symbol &) = delete;
    Symbol &operator=(Symbol &&) = delete;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_SYMBOL_H_
