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
/// \brief Buffers for source code.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONTEXT_SOURCEBUFFER_H_
#define INCLUDE_QORE_CONTEXT_SOURCEBUFFER_H_

#include <vector>
#include "qore/context/SourceId.h"

namespace qore {

/**
 * \brief Provides storage for the actual source of a script.
 *
 * Creating copies is forbidden, use `std::move` when necessary.
 * \invariant `data` is nonempty
 * \invariant last element of `data` is '\0'
 */
class SourceBuffer {

public:
    /**
     * \brief Default move constructor.
     */
    SourceBuffer(SourceBuffer &&) noexcept = default;

    /**
     * \brief Default move assignment.
     */
    SourceBuffer &operator=(SourceBuffer &&) noexcept = default;

private:
    template<typename Iterator>
    SourceBuffer(SourceId sourceId, Iterator begin, Iterator end) : sourceId(sourceId), data(begin, end) {
        this->data.push_back('\0');
    }

    explicit SourceBuffer(SourceId sourceId) : sourceId(sourceId), isStdin(true) {
        data.push_back('\n');
        data.push_back('\0');
    }

    SourceBuffer(const SourceBuffer &) = delete;
    SourceBuffer &operator=(const SourceBuffer &) = delete;

private:
    const SourceId sourceId;
    std::vector<char> data;
    bool isStdin{false};

    friend class SourceManager;         //!< Only SourceManager can create instances.
    friend class SourcePointer;         //!< SourcePointer provides random access to the buffer.
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_SOURCEBUFFER_H_
