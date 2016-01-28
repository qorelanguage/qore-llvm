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
/// \brief Core exceptions.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMMON_EXCEPTIONS_H_
#define INCLUDE_QORE_COMMON_EXCEPTIONS_H_

#include <exception>
#include <string>
#include <sstream>
#include "qore/common/Logging.h"

/**
 * \brief Marks unreachable point in the code.
 *
 * In debug version, prints a message and aborts. In release version has undefined behavior.
 * \param M a message to print
 */
#ifdef QORE_COVERAGE
class Unreachable {};
#define QORE_UNREACHABLE(M) throw Unreachable();
#elif !defined(NDEBUG)
#define QORE_UNREACHABLE(M)  CLOG("", "FATAL: Unreachable executed in " << __PRETTY_FUNCTION__ << " - " << M); abort();
#else
#define QORE_UNREACHABLE(M) __builtin_unreachable()
#endif

/**
 * \brief Helper macro for constructing messages for FatalError exception.
 *
 * Example:
 *
 *     throw FATAL_ERROR("Error test() called with " << argument << " but " << expected << " was expected.");
 */
#define FATAL_ERROR(M)  FatalError(static_cast<std::ostringstream&>(std::ostringstream().flush() << M).str());

namespace qore {

/**
 * \brief Exception class for reporting fatal errors.
 */
class FatalError : public std::exception {

public:
    /**
     * \brief Constructs the exception.
     * \param message the message of the exception
     */
    explicit FatalError(std::string message) : message(std::move(message)) {
    }

    /**
     * \brief Returns the exception message.
     * \return the message of the exception
     */
    const char* what() const noexcept override {
        return message.c_str();
    }

private:
    std::string message;
};

} // namespace qore

#endif /* INCLUDE_QORE_COMMON_EXCEPTIONS_H_ */