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
/// \brief Runtime representation of a local variable.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_LOCALVARIABLE_H_
#define INCLUDE_QORE_CORE_LOCALVARIABLE_H_

#include <memory>
#include <string>
#include "qore/core/Size.h"
#include "qore/core/Type.h"

namespace qore {

/**
 * \brief Runtime representation of a local variable.
 */
class LocalVariable {

public:
    using Ptr = std::unique_ptr<LocalVariable>;         //!< Pointer type.

public:
    /**
     * \brief Creates a local variable.
     * \param index the index of the local variable
     * \param name the name of the local variable
     * \param type the type of the local variable
     */
    LocalVariable(Size index, std::string name, const Type &type) : index(index), name(std::move(name)), type(type) {
    }

    /**
     * \brief Returns the index of the local variable.
     * @return the index of the local variable
     */
    Size getIndex() const {
        return index;
    }

    /**
     * \brief Returns the name of the local variable.
     * @return the name of the local variable
     */
    const std::string &getName() const {
        return name;
    }

    /**
     * \brief Returns the type of the local variable.
     * @return the type of the local variable
     */
    const Type &getType() const {
        return type;
    }

private:
    Size index;
    std::string name;
    const Type &type;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_LOCALVARIABLE_H_
