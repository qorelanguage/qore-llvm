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
/// \brief Defines the structure of diagnostic messages.
///
//------------------------------------------------------------------------------
#include "qore/comp/DiagRecord.h"
#include "qore/core/util/Debug.h"

namespace qore {
namespace comp {

std::ostream &operator<<(std::ostream &o, DiagLevel level) {
    switch (level) {
        case DiagLevel::Error:
            return o << "error";
        case DiagLevel::Warning:
            return o << "warning";
        case DiagLevel::Info:
            return o << "info";
    }
    QORE_UNREACHABLE("Invalid value of DiagLevel: " << static_cast<int>(level));
}

std::ostream &operator<<(std::ostream &o, DiagId id) {
    switch (id) {
        #define DIAG(N, C, L, D)        case DiagId::N: return o << #N;
        /// \cond IGNORED_BY_DOXYGEN
        #include "qore/comp/DiagData.inc"
        /// \endcond
        #undef DIAG
    }
    QORE_UNREACHABLE("Invalid value of DiagId: " << static_cast<int>(id));
}

} // namespace comp
} // namespace qore
