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
#ifndef INCLUDE_QORE_CONTEXT_SOURCE_H_
#define INCLUDE_QORE_CONTEXT_SOURCE_H_

#include <string>
#include <vector>
#include "qore/context/SourceId.h"
#include "qore/context/SourceLocation.h"

namespace qore {

/**
 * \brief Represents the source of a script.
 *
 * \invariant `index` &lt; `buf.size()` unless `buf.empty()`, in which case `index` == `0`
 */
class Source {

public:
    /**
     * \brief The return value of read() representing the end of the source
     */
    constexpr static int EndOfFile = -1;

public:
    /**
     * \brief Creates a new source.
     * \param name the name of the source
     * \param id the id of the source
     * \param buf the source buffer
     * \param stdin `true` if additional lines should be read from stdin
     */
    Source(std::string name, SourceId id, std::vector<char> buf, bool stdin) : name(std::move(name)),
            id(id), buf(std::move(buf)), stdin(stdin) {
    }

    /**
     * \brief Returns the name of the source.
     * \return the name of the source
     */
    const std::string &getName() const {
        return name;
    }

    /**
     * \brief Returns the location of the next character returned by read().
     * \return the current location
     */
    SourceLocation getLocation() const {
        return SourceLocation(id, line, column);
    }

    /**
     * \brief Reads the next character from the source.
     * \return the next character in the source or \ref EndOfFile if the end of the source has been reached
     */
    int read() {
        if (buf.empty() && stdin) {
            fill();
        }
        if (buf.empty()) {
            return EndOfFile;
        }
        char c = buf[index++];
        //XXX handle CRLF, CR, LF
        //XXX handle \t
        if (c == '\n') {
            ++line;
            column = 1;
        } else {
            ++column;
        }
        if (index == buf.size()) {
            index = 0;
            buf.clear();
        }
        return c;
    }

private:
    Source(const Source &) = delete;
    Source(Source &&) = delete;
    Source &operator=(const Source &) = delete;
    Source &operator=(Source &&) = delete;

    void fill();

private:
    std::string name;
    SourceId id;
    int line{1};
    int column{1};
    std::vector<char> buf;
    std::vector<char>::size_type index{0};
    bool stdin{false};
};

/**
 * \brief A helper class that allows peeking one character ahead in a source.
 * \tparam Src the type of the underlying source
 */
template<typename Src>
class Lookahead {

public:
    /**
     * \brief Creates the instance.
     * \param src the underlying source
     */
    template<typename... _Args>
    Lookahead(_Args&&... __args) : src(__args...), savedChar(Empty) {
    }

    /**
     * \brief Reads the next character from the source (and advances the input).
     * \return the next character in the source
     */
    int read() {
        if (savedChar != Empty) {
            int c = savedChar;
            savedChar = Empty;
            return c;
        }
        return src.read();
    }

    /**
     * \brief Peeks the next character from the source (does not advance the input).
     * \return the next character in the source
     */
    int peek() {
        if (savedChar == Empty) {
            savedLocation = src.getLocation();
            savedChar = src.read();
        }
        return savedChar;
    }

    /**
     * \brief Returns the location of the next character returned by read() or peek().
     * \return the current location
     */
    SourceLocation getLocation() const {
        return savedChar == Empty ? src.getLocation() : savedLocation;
    }

private:
    constexpr static int Empty = -2;

private:
    Src src;
    int savedChar;
    SourceLocation savedLocation;
};


} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_SOURCE_H_
