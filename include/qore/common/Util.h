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
#include <sstream>
#include <string>

namespace qore {
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

} // namespace util
} // namespace qore

#endif /* INCLUDE_QORE_COMMON_UTIL_H_ */
