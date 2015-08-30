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
/// \brief Random access pointer into a SourceBuffer.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONTEXT_SOURCEPOINTER_H_
#define INCLUDE_QORE_CONTEXT_SOURCEPOINTER_H_

#include <cassert>
#include "qore/context/SourceBuffer.h"
#include "qore/context/SourceLocation.h"

namespace qore {

/**
 * \brief Provides direct random access into a SourceBuffer.
 */
class SourcePointer {

public:
    /**
     * \brief Creates an instance pointing at the first character in the source.
     *
     * The SourcePointer instance does not own the source buffer, the caller must make sure that the buffer
     * is not destroyed before the pointer.
     * \param sourceBuffer the source buffer into which this pointer points
     */
    SourcePointer(SourceBuffer *sourceBuffer) : sourceBuffer(sourceBuffer) {
        assert(sourceBuffer != nullptr && "No buffer provided");
    }

    /**
     * \brief Returns a pointer into the buffer at current position.
     *
     * The pointer must not be used after the underlying SourceBuffer is destroyed. Moreover, the pointer
     * may become invalid when a newline character is consumed by either operator++().
     */
    operator const char *() {
        return &sourceBuffer->data[index];
    }

    /**
     * \brief Advances the source to the next character.
     *
     * Does nothing if the end of the source has already been reached.
     * \return this after advancing the pointer
     */
    SourcePointer &operator++() {
        char c = sourceBuffer->data[index];
        if (c == '\n') {
            if (sourceBuffer->isStdin) {
                fill();
            }
            line++;
            column = 0;
        }
        if (c) {
            ++index;
            ++column;
        }
        return *this;
    }

    /**
     * \brief Advances the source to the next character.
     *
     * Does nothing if the end of the source has already been reached.
     * \return this before advancing the pointer
     */
    SourcePointer operator++(int) {
        SourcePointer copy = *this;
        ++*this;
        return copy;
    }

    /**
     * \brief Returns the location of the current character.
     * \return the location of the current character
     */
    SourceLocation getLocation() {
        return SourceLocation(sourceBuffer->sourceId, line, column);
    }

private:
    void fill();

    SourceBuffer *sourceBuffer;
    std::vector<char>::size_type index{0};
    int line{1};
    int column{1};

    friend class SourcePointerTestHelper;
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_SOURCEPOINTER_H_
