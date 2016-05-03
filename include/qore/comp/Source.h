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
/// \brief Representation of script sources.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SOURCE_H_
#define INCLUDE_QORE_COMP_SOURCE_H_

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>
#include "qore/core/SourceLocation.h"

namespace qore {
namespace comp {

/**
 * \brief Represents the source of a script.
 *
 * The '\0' character is used to indicate the end of the source.
 *
 * \invariant `data` is non-empty
 * \invariant the last element of `data` is '\0'
 * \invariant there are no other '\0' characters in `data`
 * \invariant `data.begin()` &lt;= `ptr` &lt; `data.end()`
 * \invariant `ptr` points to the next character to be read
 */
class Source {

public:
    /**
     * \brief Creates a new source.
     * \param info the source info
     * \param buf the source buffer with no '\0' characters
     */
    Source(const SourceInfo &info, std::vector<char> buf) : info(info), data(std::move(buf)), line(1), column(1) {
        assert(std::find(data.begin(), data.end(), '\0') == data.end());
        data.push_back('\0');
        ptr = data.begin();
        mark = data.begin();
        markLocation = SourceLocation(info, 0, line, column);
    }

    /**
     * \brief Returns the source info.
     * \return the source info
     */
    const SourceInfo &getInfo() const {
        return info;
    }

    /**
     * \brief Reads the next character from the source and advances the pointer.
     * \return the next character in the source or '\0' if the end of the source has been reached.
     */
    char read() {
        constexpr int tabSize = 4;
        switch (*ptr) {
            case 0:
                return 0;
            case '\n':
                ++line;
                column = 1;
                break;
            case '\r':
                if (*(ptr + 1) == '\n') {
                    ++column;
                } else {
                    ++line;
                    column = 1;
                }
                break;
            case '\t':
                column += tabSize - (column - 1) % tabSize;
                break;
            default:
                ++column;
                break;
        }
        return *ptr++;
    }

    /**
     * \brief Returns the next character from the source but does not advance the pointer.
     * \return the next character in the source or '\0' if the end of the source has been reached.
     */
    char peek() const {
        return *ptr;
    }

    /**
     * \brief Unreads the last character read, moving the pointer backwards.
     *
     * The behavior is undefined if no characters were read from the source.
     */
    void unread() {
        assert(ptr != data.begin());
        --ptr;
        assert(*ptr > 32);
        --column;
    }

    /**
     * \brief Appends characters to the end of the source buffer.
     * \param string the characters to append (with no '\0' characters)
     */
    void append(const std::string &string) {
        assert(find(string.begin(), string.end(), '\0') == string.end());
        auto offset = ptr - data.begin();
        auto markOffset = mark - data.begin();
        data.insert(data.end() - 1, string.begin(), string.end());
        ptr = data.begin() + offset;
        mark = data.begin() + markOffset;
    }

    /**
     * \brief Returns `true` if the last character read is the first character on its line.
     *
     * The behavior is undefined if no characters were read from the source.
     * \return `true` if the last character read is the first character on its line
     */
    bool wasFirstOnLine() const {
        assert(ptr != data.begin());
        auto p = ptr - 1;
        if (p == data.begin()) {
            return true;
        }
        --p;
        return *p == '\n' || *p == '\r';
    }

    /**
     * \brief Marks the current location in the source.
     */
    void setMark() {
        mark = ptr;
        markLocation = SourceLocation(info, static_cast<int>(mark - data.begin()), line, column);
    }

    /**
     * \brief Returns the marked location.
     * \return the marked location
     */
    SourceLocation getMarkLocation() const {
        return markLocation;
    }

    /**
     * \brief Returns the substring from the mark up to the current location.
     * \return the substring from the mark up to the current location
     */
    std::string getMarkedString() const {
        return std::string(mark, ptr);
    }

    /**
     * \brief Returns the length of the substring from the mark up to the current location.
     * \return the length of the substring from the mark up to the current location
     */
    int getMarkedLength() const {
        return static_cast<int>(ptr - mark);
    }

    /**
     * \brief Returns the substring of given length starting at given offset.
     * \param offset the offset of the first character of the substring
     * \param length the length of the substring
     * \return the substring of `length` starting at `offset`
     */
    std::string getRange(int offset, int length) const {
        assert(offset >= 0 && length >= 0 && offset + length < static_cast<int>(data.size()));
        return std::string(data.begin() + offset, data.begin() + offset + length);
    }

private:
    Source(const Source &) = delete;
    Source(Source &&) = delete;
    Source &operator=(const Source &) = delete;
    Source &operator=(Source &&) = delete;

private:
    const SourceInfo &info;
    std::vector<char> data;
    std::vector<char>::iterator ptr;
    std::vector<char>::iterator mark;
    SourceLocation markLocation;
    int line;
    int column;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SOURCE_H_
