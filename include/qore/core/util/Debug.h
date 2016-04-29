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
/// \brief Debugging functions and macros.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_UTIL_DEBUG_H_
#define INCLUDE_QORE_CORE_UTIL_DEBUG_H_

#include "qore/core/util/Logging.h"

/**
 * \brief Marks an unimplemented point in the code.
 *
 * Prints a logging message and throws an exception that is not supposed to be caught anywhere, aborting the execution.
 * \param M the message to print
 */
#define QORE_NOT_IMPLEMENTED(M) \
            LOG(__PRETTY_FUNCTION__ << " - NOT IMPLEMENTED: " << M); \
            throw ::qore::util::NotImplemented();

/**
 * \brief Marks an unreachable point in the code.
 *
 * In debug version, prints a message and aborts. In release version it has undefined behavior.
 * \param M the message to print
 */
#define QORE_UNREACHABLE(M) \
            LOG("FATAL: Unreachable in " << __PRETTY_FUNCTION__ << " - " << M); \
            QORE_UNREACHABLE_IMPL;

namespace qore {
namespace util {

/**
 * \brief An exception class thrown by \ref QORE_NOT_IMPLEMENTED.
 */
class NotImplemented {};

#ifdef QORE_COVERAGE
/**
 * \brief An exception class thrown by \ref QORE_UNREACHABLE.
 */
class Unreachable {};
#endif // QORE_COVERAGE

} // namespace util
} // namespace qore

/// \cond NoDoxygen
#ifdef QORE_COVERAGE
#define QORE_UNREACHABLE_IMPL   throw ::qore::util::Unreachable();
#elif !defined(NDEBUG)
#define QORE_UNREACHABLE_IMPL   abort();
#else
#define QORE_UNREACHABLE_IMPL   __builtin_unreachable()
#endif
/// \endcond NoDoxygen

#endif // INCLUDE_QORE_CORE_UTIL_DEBUG_H_
