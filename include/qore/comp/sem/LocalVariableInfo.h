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
/// \brief Defines the LocalVariableInfo class.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEM_LOCALVARIABLEINFO_H_
#define INCLUDE_QORE_COMP_SEM_LOCALVARIABLEINFO_H_

#include <memory>
#include "qore/core/LocalVariable.h"

namespace qore {
namespace comp {
namespace sem {

/**
 * \brief Describes a local variable during semantic analysis.
 */
class LocalVariableInfo {

public:
    using Ptr = std::unique_ptr<LocalVariableInfo>;     //!< Pointer type.

public:
    /**
     * \brief Constructor.
     * \param name the name of the local variable
     * \param location the location of the local variable
     * \param type the type of the local variable
     */
    LocalVariableInfo(String::Ref name, SourceLocation location, const Type &type) : name(name),
            location(location), type(type), rt(nullptr) {
    }

    /**
     * \brief Returns the name of the local variable.
     * \return the name of the local variable
     */
    String::Ref getName() const {
        return name;
    }

    /**
     * \brief Returns the location of the local variable.
     * \return the location of the local variable
     */
    SourceLocation getLocation() const {
        return location;
    }

    /**
     * \brief Returns the type of the local variable.
     * \return the type of the local variable
     */
    const Type &getType() const {
        return type;
    }

    /**
     * \brief Returns the runtime object representing the local variable.
     *
     * This method can be called only if \ref setRt() has been called.
     * \return the runtime object representing the local variable
     */
    LocalVariable &getRt() const {
        assert(rt);
        return *rt;
    }

    /**
     * \brief Sets the runtime object representing the local variable.
     *
     * This is usually called after the first pass, when it is known which local variables are shared (from closures
     * or via references).
     * \param rt the runtime object representing the local variable
     */
    void setRt(LocalVariable &rt) {
        assert(!this->rt);
        this->rt = &rt;
    }

private:
    String::Ref name;
    SourceLocation location;
    const Type &type;
    LocalVariable *rt;
};

} // namespace sem
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEM_LOCALVARIABLEINFO_H_
