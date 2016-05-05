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
/// \brief Runtime representation of a group of function overloads.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_FUNCTIONGROUP_H_
#define INCLUDE_QORE_CORE_FUNCTIONGROUP_H_

#include <string>
#include <vector>
#include "qore/core/Function.h"
#include "qore/core/util/Iterators.h"

namespace qore {

/**
 * \brief Runtime representation of a group of function overloads.
 */
class FunctionGroup {

public:
    using Ptr = std::unique_ptr<FunctionGroup>;                                 //!< Pointer type.
    using FunctionItartor = util::VectorOfUniquePtrIteratorAdapter<Function>;   //!< Function iterator.

public:
    /**
     * \brief Creates the function group.
     * \param fullName the full name of the function group
     */
    explicit FunctionGroup(std::string fullName) : fullName(std::move(fullName)) {
        assert(this->fullName.find(':') != std::string::npos);
    }

    /**
     * \brief Returns the full name of the function group.
     * \return the full name of the function group
     */
    const std::string &getFullName() const {
        return fullName;
    }

    /**
     * \brief Creates a new function.
     * \param type the type of the function
     * \param location the location of the declaration
     * \return newly created function
     */
    Function &addFunction(FunctionType type, SourceLocation location) {
        Function::Ptr ptr = Function::Ptr(new Function(std::move(type), location));
        Function &f = *ptr;
        functions.push_back(std::move(ptr));
        return f;
    }

    /**
     * \brief Returns a range for iterating functions.
     * \return a range for iterating functions
     */
    util::IteratorRange<FunctionItartor> getFunctions() const {
        return util::IteratorRange<FunctionItartor>(functions);
    }

private:
    FunctionGroup(const FunctionGroup &) = delete;
    FunctionGroup(FunctionGroup &&) = delete;
    FunctionGroup &operator=(const FunctionGroup &) = delete;
    FunctionGroup &operator=(FunctionGroup &&) = delete;

private:
    std::string fullName;
    std::vector<Function::Ptr> functions;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_FUNCTIONGROUP_H_
