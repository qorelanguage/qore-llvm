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
#ifndef TEST_QTIF_CORE_H_
#define TEST_QTIF_CORE_H_

#include <exception>
#include <regex>
#include <string>
#include <vector>

namespace qtif {

class Exception : public std::runtime_error {

public:
    Exception(const std::string &msg, int line) : runtime_error(msg), line(line) {
    }

    int getLine() const {
        return line;
    }

private:
    int line;
};

inline std::vector<std::string> findFiles(const std::string &filter) {
    std::regex regex(filter);
    std::vector<std::string> files;
    #define QTIF(n)     if (std::regex_search(n, regex)) { files.push_back(n); }
    #include "test_files.inc"
    #undef QTIF
    return files;
}

} // namespace qtif

#endif // TEST_QTIF_CORE_H_
