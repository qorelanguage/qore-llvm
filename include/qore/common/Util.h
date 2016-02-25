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
/// \brief Utility functions
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMMON_UTIL_H_
#define INCLUDE_QORE_COMMON_UTIL_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <sstream>
#include <string>

namespace qore {

/**
 * \brief Contains utility functions.
 */
namespace util {

/**
 * \brief Trims leading and trailing characters from a string.
 * \param s the string to trim
 * \param pred a predicate for determining which characters to trim, e.g. isspace
 * \return the trimmed string
 */
template<typename Predicate>
std::string trim(const std::string &s, Predicate pred) {
    auto wsfront = std::find_if_not(s.begin(), s.end(), pred);
    auto wsback = std::find_if_not(s.rbegin(), s.rend(), pred).base();
    return wsback <= wsfront ? std::string() : std::string(wsfront, wsback);
}

/**
 * \brief Converts a value to a string.
 * \tparam T the type of the value, must support `operator<<` for std::ostream
 * \param t the value to convert
 * \return string representation of the value
 */
template<typename T>
std::string to_string(const T &t) {
    std::ostringstream str;
    str << t;
    return str.str();
}

/**
 * \brief Construct an object of type T and wraps it in a std::unique_ptr.
 * \param args list of arguments with which an instance of T will be constructed
 * \return std::unique_ptr of an instance of type T
 */
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * \brief Allows \ref util::make_unique() to create instances of a class.
 *
 * Use this macro in classes with non-public constructors to allow creating instances wrapped in a `std::unique_ptr`.
 * Example:
 * \code{.cpp}
 *   class C {
 *   private:
 *       C() {}
 *       FRIEND_MAKE_UNIQUE;
 *   };
 *   qore::util::make_unique<C>();  //OK
 *   C c;                           //error
 *   C *c = new C();                //error
 * \endcode
 */
#define FRIEND_MAKE_UNIQUE template<typename T, typename... Args> \
                           friend std::unique_ptr<T> qore::util::make_unique(Args&&...);

/**
 * \brief Helper function for safely inserting elements to a vector of unique pointers.
 *
 * Constructs an object, wraps it in a `std::unique_ptr`, inserts it in a `std::vector` and returns the naked pointer.
 * Contrary to `vector.emplace_back(new T(args...))`, this function prevents a memory leak in the case that
 * `emplace_back` throws an exception.
 * \param vector the vector to insert the new element to
 * \param args list of arguments with which an instance of T will be constructed
 * \return a pointer to the new instance of T
 */
template<typename T, typename B, typename... Args>
T *safe_emplace_back(std::vector<std::unique_ptr<B>> &vector, Args&&... args) {
    std::unique_ptr<T> ptr = make_unique<T>(std::forward<Args>(args)...);
    T *value = ptr.get();
    vector.emplace_back(std::move(ptr));
    return value;
}

} // namespace util
} // namespace qore

#endif /* INCLUDE_QORE_COMMON_UTIL_H_ */
