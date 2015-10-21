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
#include "AbstractTest.h"
#include <fstream>

namespace qtif {

void AbstractTest::SetUp() {
    setUpDone = false;
    fileName = GetParam();
    try {
        input = readFile(fileName);
        Reader reader(input.begin(), input.end());
        int patternPos = findSeparator(reader);
        parseExpectations(reader);
        input.resize(patternPos);
        input.shrink_to_fit();
        setUpDone = true;
    } catch (Exception &e) {
        QTIF_FATAL(e.getLine()) << e.what();
    }
}

std::vector<char> AbstractTest::readFile(const std::string &fileName) {
    std::ifstream fileStream(fileName, std::ios::binary);
    std::vector<char> data{std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>()};
    if (!fileStream) {
        throw Exception("unable to read input file", 1);
    }
    return data;
}

int AbstractTest::findSeparator(Reader &reader) {
    static const std::string separator = "#$$$";

    std::string::size_type posInPattern = 0;
    while (!reader.eof()) {
        if (reader.read() == separator[posInPattern]) {
            if (++posInPattern == separator.length()) {
                int patternPos = reader.getPos() - separator.length();
                reader.skipIf('\r');
                reader.skipIf('\n');
                return patternPos;
            }
        } else {
            posInPattern = 0;
        }
    }
    throw Exception("missing separator " + separator, reader.getLineNumber());
}

} // namespace qtif
