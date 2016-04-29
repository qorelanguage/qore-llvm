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
/// \brief Utility interface for logging.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_UTIL_LOGGABLE_H_
#define INCLUDE_QORE_CORE_UTIL_LOGGABLE_H_

#ifdef QORE_LOGGING

#include <ostream>

/**
 * \brief Conditionally overrides the qore::Loggable::writeToLog() method.
 * \param l the expression to be written to the output stream
 */
#define WRITE_TO_LOG(l) std::ostream &writeToLog(std::ostream &os) const override { return os << l; }

#else

#define WRITE_TO_LOG(_)

#endif

namespace qore {
namespace util {

/**
 * \brief Provides logging support for polymorphic class hierarchies.
 *
 * Classes implementing this interface can be written to any std::ostream using the `<<` operator when
 * the `QORE_LOGGING` macro is defined. Otherwise does nothing. Override the writeToLog() method to customize the
 * actual output, but only if `QORE_LOGGING` is defined. Simple implementations can use the \ref WRITE_TO_LOG macro.
 * Example:
 *
 *     class C : public Loggable {
 *     protected:
 *         WRITE_TO_LOG("instance of C, member = " << member)
 *
 *     private:
 *         int member;
 *     };
 */
class Loggable {
#ifdef QORE_LOGGING
protected:
    Loggable() {
    }

    /**
     * \brief Writes a textual representation to an output stream.
     * \param os the output stream
     * @return the output stream
     */
    virtual std::ostream &writeToLog(std::ostream &os) const {
        return os << "abstract";
    }

private:
    friend std::ostream &operator<<(std::ostream &, const Loggable &);
#endif
};

#ifdef QORE_LOGGING

/**
 * \brief Writes a Loggable object to an output stream.
 * \param os the output stream
 * \param obj the object to write
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const Loggable &obj) {
    return obj.writeToLog(os) << " <" << static_cast<const void*>(&obj) << ">";
}

/**
 * \brief Writes a Loggable object to an output stream.
 * \param os the output stream
 * \param ptr a pointer to the object to write (can be `nullptr`)
 * \return the output stream
 */
inline std::ostream &operator<<(std::ostream &os, const Loggable *ptr) {
    return ptr ? os << *ptr : os << "<nullptr>";
}
#endif

} // namespace util
} // namespace qore

#endif // INCLUDE_QORE_CORE_UTIL_LOGGABLE_H_
