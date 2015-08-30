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
/// \brief Declares opaque source identifier type.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONTEXT_SOURCEID_H_
#define INCLUDE_QORE_CONTEXT_SOURCEID_H_

#include <cassert>
#include "qore/common/Util.h"

namespace qore {

/**
 * \brief Identifies a script source in the source manager.
 */
class SourceId {

public:
    const static SourceId Invalid;      //!< Represents an invalid source identifier.

    /**
     * \brief Compares two source identifiers for equality.
     * \param other the SourceId `this` should be compared to
     * \return true if `this` equals `other`
     */
    bool operator==(const SourceId &other) {
        return id == other.id;
    }

    /**
     * \brief Compares two source identifiers for inequality.
     * \param other the SourceId `this` should be compared to
     * \return true if `this` does not equal `other`
     */
    bool operator!=(const SourceId &other) {
        return id != other.id;
    }

private:
    SourceId() : id(-1) {
    }

    explicit SourceId(short id) : id(id) {
        assert(id >= 0 && "SourceId must not be negative");
    }

private:
    short id;

    friend class SourceManager;         //!< Only SourceManager can create instances.
    FRIEND_FIXTURE(SourceIdTest);
    FRIEND_FIXTURE(SourceBufferTest);
    FRIEND_TEST(SourceIdDeathTest, CtorChecksNegative);
    FRIEND_TEST(SourceIdDeathTest, CtorChecksOutOfRange);
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_SOURCEID_H_
