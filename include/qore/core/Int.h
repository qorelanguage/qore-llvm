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
/// \brief Defines the String class which implements a wrapper around Qore's `int` type.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CORE_INT_H_
#define INCLUDE_QORE_CORE_INT_H_

#include "qore/core/Any.h"
#include "qore/core/Value.h"

namespace qore {

/**
 * \brief Wrapper for Qore's `int` type.
 */
class Int : public Any {

public:
    /**
     * \brief Creates a new instance.
     * \param i the value
     */
    explicit Int(qint i) : i(i) {
        LOG(this << " created");
    }

    const Type &getType() const override {
        return Type::Int;
    }

    /**
     * \brief Returns the value.
     * \return the value
     */
    qint get() const {
        return i;
    }

protected:
    ~Int() {
        LOG(this << " destroyed");
    }

protected:
    WRITE_TO_LOG("Int \"" << i << "\"")

private:
    qint i;
};

} // namespace qore

#endif // INCLUDE_QORE_CORE_INT_H_
