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
#ifndef TEST_UTILS_H_
#define TEST_UTILS_H_

namespace qore {

class RedirectStdin {
public:
    RedirectStdin(std::string string) : stream(string), cin_backup(std::cin.rdbuf(stream.rdbuf())) {
    }

    ~RedirectStdin() {
        std::cin.rdbuf(cin_backup);
    }

private:
    std::istringstream stream;
    std::streambuf* cin_backup;
};

class RedirectStderr {
public:
    RedirectStderr(std::string &dest) : dest(dest), cerr_backup(std::cerr.rdbuf(stream.rdbuf())) {
    }

    ~RedirectStderr() {
        std::cerr.rdbuf(cerr_backup);
        dest = stream.str();
    }

private:
    std::string &dest;
    std::ostringstream stream;
    std::streambuf* cerr_backup;
};

} // namespace qore

#endif // TEST_UTILS_H_
