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
/// \brief Source functions.
///
//------------------------------------------------------------------------------
#include "qore/comp/Source.h"

namespace qore {
namespace comp {

std::pair<int, int> Source::decodeLocation(int offset, int tabSize) {
    assert(tabSize > 0);

    if (offset < 0 || static_cast<std::vector<char>::size_type>(offset) >= data.size()) {
        return std::make_pair(0, 0);
    }

    int line = 1;
    int column = 1;

    auto end = data.begin() + offset;

    for (auto it = data.begin(); it != end; ++it) {
        int c = *it;

        if ((c == '\r' && *(it + 1) != '\n') || c == '\n') {
                         //(it + 1) is guaranteed to be valid, since data ends with '\0'
            ++line;
            column = 1;
        } else if (*it == '\t') {
            column += tabSize - (column - 1) % tabSize;
        } else {
            ++column;
        }
    }

    //XXX line offsets can be cached
    return std::make_pair(line, column);
}

} // namespace comp
} // namespace qore
