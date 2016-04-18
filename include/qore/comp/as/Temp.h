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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_AS_TEMP_H_
#define INCLUDE_QORE_COMP_AS_TEMP_H_

#include "qore/Id.h"

namespace qore {
namespace comp {
namespace as {

class Temp {

public:
    explicit Temp(Id index) : index(index) {
    }

    Id getIndex() const {
        return index;
    }

    bool isValid() const {
        return index != InvalidId;
    }

    Temp(const Temp &) = default;
    Temp(Temp &&) = default;
    Temp &operator=(const Temp &) = default;
    Temp &operator=(Temp &&) = default;

private:
    Id index;
};

} // namespace as
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_AS_TEMP_H_