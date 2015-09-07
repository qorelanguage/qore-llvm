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
/// \brief Management of sources, locations, buffers and pointers.
///
//------------------------------------------------------------------------------
#include "qore/common/Util.h"
#include "qore/context/SourcePointer.h"
#include "qore/context/SourceManager.h"
#include <fstream>
#include <iostream>

namespace qore {

const SourceId SourceId::Invalid;

void SourcePointer::fill() {
    std::string line;
    std::getline(std::cin, line);
    if (std::cin.good()) {
        line.push_back('\n');
        sourceBuffer.data.insert(sourceBuffer.data.end() - 1, line.begin(), line.end());
    }
}

SourceBuffer SourceManager::createFromFile(std::string fileName) {
    std::ifstream fileStream(fileName, std::ios::binary);
    std::vector<char> data{std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>()};
    if (!fileStream.good()) {
        throw FATAL_ERROR("Error reading file " << fileName);
    }
    return SourceBuffer(createId(std::move(fileName)), std::move(data));
}

} // namespace qore
