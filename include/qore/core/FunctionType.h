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
/// \brief Defines function type.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_FUNCTIONTYPE_H_
#define INCLUDE_QORE_CORE_FUNCTIONTYPE_H_

#include <vector>
#include "qore/core/Size.h"
#include "qore/core/Type.h"

namespace qore {

/**
 * \brief Describes the type of a function - the types of its parameters and the type of the return value.
 */
class FunctionType {

public:
    /**
     * \brief Creates a function type with no parameters.
     * \param returnType the type of the function's return value
     */
    explicit FunctionType(const Type &returnType) : returnType(returnType) {
    }

    /**
     * \brief Default move constructor.
     * \param src the source instance
     */
    FunctionType(FunctionType &&src) = default;

    /**
     * \brief Default move assignment.
     * \param src the source instance
     * \return this instance
     */
    FunctionType &operator=(FunctionType &&src) = default;

    /**
     * \brief Returns the type of the function's return value.
     * \return the type of the function's return value
     */
    const Type &getReturnType() const {
        return returnType;
    }

    /**
     * \brief Adds a parameter of the function.
     * \param type the type of the parameter
     */
    void addParameter(const Type &type) {
        paramTypes.push_back(&type);
    }

    /**
     * \brief Returns the number of parameters.
     * \return the number of parameters
     */
    Size getParameterCount() const {
        return paramTypes.size();
    }

private:
    FunctionType(const FunctionType &) = delete;
    FunctionType &operator=(const FunctionType &) = delete;

private:
    const Type &returnType;
    std::vector<const Type *> paramTypes;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_FUNCTIONTYPE_H_
