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

namespace qore {
namespace comp {

/**
 * \brief Represents a location in the source code.
 */
struct SourceLocation {

public:
    int sourceId;           //!< Id of the source.
    int offset;             //!< Offset in the source.

public:
    /**
     * \brief Constructs an empty location instance.
     *
     * The instance should be filled in later using copy assignment.
     */
    SourceLocation() : sourceId(-1), offset(-1) {
    }

    /**
     * \brief Constructs a location with given parameters.
     * \param sourceId the source id
     * \param offset the offset in the source
     */
    SourceLocation(int sourceId, int offset) : sourceId(sourceId), offset(offset) {
    }
};

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
     * \param name the name of the source
     * \param id the id of the source
     * \param buf the source buffer with no '\0' characters
     */
    Source(std::string name, int id, std::vector<char> buf) : name(std::move(name)), id(id), data(std::move(buf)) {
        assert(std::find(data.begin(), data.end(), '\0') == data.end());
        data.push_back('\0');
        ptr = data.begin();
        mark = data.begin();
    }

    /**
     * \brief Returns the name of the source.
     * \return the name of the source
     */
    const std::string &getName() const {
        return name;
    }

    /**
     * \brief Returns the id of the source.
     * \return the id of the source
     */
    int getId() const {
        return id;
    }

    /**
     * \brief Reads the next character from the source and advances the pointer.
     * \return the next character in the source or '\0' if the end of the source has been reached.
     */
    char read() {
        return *ptr ? *ptr++ : *ptr;
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
    }

    /**
     * \brief Appends characters to the end of the source buffer.
     * \param string the characters to append (with no '\0' characters)
     */
    void append(const std::string &string) {
        assert(find(string.begin(), string.end(), '\0') == string.end());
        auto offset = ptr - data.begin();
        auto markOffset = ptr - data.begin();
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
    }

    /**
     * \brief Returns the marked location.
     * \return the marked location
     */
    SourceLocation getMarkLocation() const {
        return SourceLocation(getId(), static_cast<int>(mark - data.begin()));
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
     * \brief Converts an offset in the source into a (line, column) pair.
     * \param offset in the source
     * \param tabSize size of horizontal tab
     * \return a (line, column) pair corresponding to the offset or (0, 0) if `offset` is out of bounds
     */
    std::pair<int, int> decodeLocation(int offset, int tabSize = 4);

private:
    Source(const Source &) = delete;
    Source(Source &&) = delete;
    Source &operator=(const Source &) = delete;
    Source &operator=(Source &&) = delete;

private:
    std::string name;
    int id;
    std::vector<char> data;
    std::vector<char>::iterator ptr;
    std::vector<char>::iterator mark;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SOURCE_H_
