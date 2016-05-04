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
/// \brief Runtime representation of a class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_CLASS_H_
#define INCLUDE_QORE_CORE_CLASS_H_

#include <memory>
#include <string>
#include "qore/core/SourceLocation.h"

namespace qore {

/**
 * \brief Runtime representation of a class.
 */
class Class {

public:
    using Ptr = std::unique_ptr<Class>;         //!< Pointer type.

public:
    /**
     * \brief Creates the class.
     * \param name the name of the class
     * \param location the location of the declaration in the source
     */
    Class(std::string name, SourceLocation location) : name(std::move(name)), location(location) {
    }

    /**
     * \brief Returns the name of the class.
     * \return the name of the class
     */
    const std::string &getName() const {
        return name;
    }

    /**
     * \brief Returns the location of the declaration.
     * \return the location of the declaration
     */
    const SourceLocation &getLocation() const {
        return location;
    }

private:
    Class(const Class &) = delete;
    Class(Class &&) = delete;
    Class &operator=(const Class &) = delete;
    Class &operator=(Class &&) = delete;

private:
    std::string name;
    SourceLocation location;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_CLASS_H_
