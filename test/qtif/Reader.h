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
#ifndef TEST_QTIF_READER_H_
#define TEST_QTIF_READER_H_

#include <string>
#include <utility>
#include <vector>

namespace qtif {

class Reader {

public:
    using Iterator = std::vector<char>::iterator;

    Reader(Iterator begin, Iterator end) : begin(begin), it(begin), end(end), line(1), wasCr(false) {
    }

    int read() {
        if (it == end) {
            return -1;
        }
        int c = *it++;
        if (c == '\r' || (c == '\n' && !wasCr)) {
            ++line;
        }
        wasCr = c == '\r';
        return c;
    }

    bool eof() const {
        return it == end;
    }

    int getLineNumber() const {
        return line;
    }

    int getPos() const {
        return static_cast<int>(it - begin);
    }

    void skipIf(int c) {
        if (it != end && *it == c) {
            read();
        }
    }

    template<typename C>
    C getRest() {
        return C(it, end);
    }

    std::pair<int, std::string> readLine() {
        std::pair<int, std::string> result;
        result.first = line;
        while (!eof()) {
            int c = read();
            if (c == '\r') {
                skipIf('\n');
                break;
            }
            if (c == '\n') {
                break;
            }
            result.second.push_back(c);
        }
        return result;
    }

private:
    Iterator begin;
    Iterator it;
    Iterator end;
    int line;
    bool wasCr;
};

} // namespace qtif

#endif // TEST_QTIF_READER_H_
