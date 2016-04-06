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
/// \brief String table.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_STRING_H_
#define INCLUDE_QORE_COMP_STRING_H_

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>

namespace qore {
namespace comp {

/**
 * \brief Represents a string in the string table.
 */
class String {

public:
    /**
     * \brief Reference to a string in the string table.
     */
    using Ref = uint64_t;

private:
    String() = delete;
};

/**
 * \brief A simple string table.
 */
class StringTable {

public:
    /**
     * \brief Puts a string in the table and returns its reference.
     * \param str the string to add to the table
     * \return the string reference
     */
    String::Ref put(const std::string &str) {
        String::Ref &value = map[str];
        if (!value) {
            data.emplace_back(str);
            value = data.size();
        }
        return value;
    }

    /**
     * \brief Returns the string identified by given reference.
     * \param ref a valid reference to the string table
     * \return the string value
     */
    const std::string &get(String::Ref ref) {
        assert(ref > 0 && ref <= data.size());
        return data[ref - 1];
    }

private:
    std::vector<std::string> data;
    std::unordered_map<std::string, String::Ref> map;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_STRING_H_
