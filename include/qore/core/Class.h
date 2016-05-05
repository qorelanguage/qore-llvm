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

#include <cassert>
#include <memory>
#include <string>
#include "qore/core/SourceLocation.h"
#include "qore/core/Type.h"

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
     * \param fullName the full name of the class
     * \param location the location of the declaration in the source
     */
    Class(std::string fullName, SourceLocation location) : fullName(std::move(fullName)), location(location) {
        assert(this->fullName.find(':') != std::string::npos);
        type = Type::createForClass(this->fullName, false);
        typeOpt = Type::createForClass(this->fullName, true);
    }

    /**
     * \brief Returns the full name of the class.
     * \return the full name of the class
     */
    const std::string &getFullName() const {
        return fullName;
    }

    /**
     * \brief Returns the location of the declaration.
     * \return the location of the declaration
     */
    const SourceLocation &getLocation() const {
        return location;
    }

    /**
     * \brief Returns the type this class defines.
     * \param optional true if the `*C` should be returned, false for just `C`
     * \return the type defined by this class
     */
    const Type &getType(bool optional) const {
        return optional ? *typeOpt : *type;
    }

private:
    Class(const Class &) = delete;
    Class(Class &&) = delete;
    Class &operator=(const Class &) = delete;
    Class &operator=(Class &&) = delete;

private:
    std::string fullName;
    SourceLocation location;
    Type::Ptr type;
    Type::Ptr typeOpt;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_CLASS_H_
