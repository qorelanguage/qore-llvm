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
/// \brief Runtime representation of a global variable.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_GLOBALVARIABLE_H_
#define INCLUDE_QORE_CORE_GLOBALVARIABLE_H_

#include <string>
#include "qore/core/SourceLocation.h"
#include "qore/core/Type.h"
#include "qore/core/Value.h"

namespace qore {

/**
 * \brief Runtime representation of a global variable.
 *
 * A global variable is created without value. It is illegal to call getValue() or setValue() on such instance.
 * The initValue() method must be used to set the initial value of the global variable. This design was necessary
 * because some Qore types do not have any default value.
 * \todo implement locking
 */
class GlobalVariable {

public:
    using Ptr = std::unique_ptr<GlobalVariable>;        //!< Pointer type.

public:
    /**
     * \brief Creates the global variable.
     * \param fullName the full name of the variable
     * \param type the type of the variable
     * \param location the location of the declaration in the source
     */
    GlobalVariable(std::string fullName, const Type &type, SourceLocation location) : fullName(std::move(fullName)),
            type(type), location(location), hasValue(false) {
        assert(this->fullName.find(':') != std::string::npos);
    }

    /**
     * \brief Decreases the reference count of the value (if needed).
     */
    ~GlobalVariable() {
        if (hasValue && type.isRefCounted() && value.p) {
            value.p->decRefCount();
        }
        LOG("GlobalVariable " << fullName << " destroyed");
    }

    /**
     * \brief Returns the full name of the global variable.
     * \return the full name of the global variable
     */
    const std::string &getFullName() const {
        return fullName;
    }

    /**
     * \brief Returns the type of the global variable.
     * \return the type of the global variable
     */
    const Type &getType() const {
        return type;
    }

    /**
     * \brief Returns the location of the declaration.
     * \return the location of the declaration
     */
    const SourceLocation &getLocation() const {
        return location;
    }

    /**
     * \brief Acquires the read lock for the global variable.
     */
    void readLock() {
        LOG("GlobalVariable " << fullName << " read lock");
    }

    /**
     * \brief Releases the read lock for the global variable.
     */
    void readUnlock() {
        LOG("GlobalVariable " << fullName << " read unlock");
    }

    /**
     * \brief Acquires the write lock for the global variable.
     */
    void writeLock() {
        LOG("GlobalVariable " << fullName << " write lock");
    }

    /**
     * \brief Releases the write lock for the global variable.
     */
    void writeUnlock() {
        LOG("GlobalVariable " << fullName << " write unlock");
    }

    /**
     * \brief Returns the value of the global variable.
     *
     * The thread must acquire the read or write lock before calling this method. Also, the global variable must be
     * initialized first by calling the initValue() method.
     * \return the value of the global variable
     */
    qvalue getValue() {
        assert(hasValue);
        //assert read or write lock acquired by current thread
        LOG("GlobalVariable " << fullName << " get");
        return value;
    }

    /**
     * \brief Sets the value of the global variable.
     *
     * The thread must acquire the write lock before calling this method. Also, the global variable must be
     * initialized first by calling the initValue() method.
     * \param v the new value of the global variable
     */
    void setValue(qvalue v) {
        assert(hasValue);
        //assert write lock acquired by current thread
        LOG("GlobalVariable " << fullName << " set");
        value = v;
    }

    /**
     * \brief Initializes the global variable by setting its initial value.
     *
     * Unlike setValue(), this method does not require that the thread acquires the write lock. It is assumed that
     * this method is called from an initialization function (synthesized by the compiler) before any user threads
     * are created.
     * \param v the value of the global variable
     */
    void initValue(qvalue v) {
        assert(!hasValue);
        LOG("GlobalVariable " << fullName << " created");
        hasValue = true;
        value = v;
    }

private:
    GlobalVariable(const GlobalVariable &) = delete;
    GlobalVariable(GlobalVariable &&) = delete;
    GlobalVariable &operator=(const GlobalVariable &) = delete;
    GlobalVariable &operator=(GlobalVariable &&) = delete;

private:
    std::string fullName;
    const Type &type;
    SourceLocation location;
    bool hasValue;
    qvalue value;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_GLOBALVARIABLE_H_
