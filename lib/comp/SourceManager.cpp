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
/// \brief Management of script sources.
///
//------------------------------------------------------------------------------
#include "qore/comp/SourceManager.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "qore/core/util/Util.h"

namespace qore {
namespace comp {

Source &SourceManager::create(const SourceInfo &info, std::vector<char> data) {
    assert(sources.find(&info) == sources.end());

    bool hasNulls = false;
    for (auto it = std::find(data.begin(), data.end(), 0); it != data.end(); it = std::find(it, data.end(), 0)) {
        hasNulls = true;
        *it = ' ';
    }
    if (hasNulls) {
        diagMgr.report(DiagId::CompNulCharactersIgnored, SourceLocation(info, 1, 1));
    }

    std::unique_ptr<Source> ptr = util::make_unique<Source>(info, std::move(data));
    Source &src = *ptr;
    sources[&info] = std::move(ptr);
    return src;
}

Source &SourceManager::createFromFile(SourceInfo &info, std::string fileName, SourceLocation location) {
    std::string fullName = includePath + fileName;
    std::ifstream fileStream(fullName, std::ios::binary);
    std::vector<char> data{std::istreambuf_iterator<char>(fileStream), std::istreambuf_iterator<char>()};
    if (!fileStream.good()) {
        diagMgr.report(DiagId::CompScriptFileIoError, location) << fullName;
        data.clear();
    }
    info.setFullName(std::move(fullName));
    return create(info, std::move(data));
}

} // namespace comp
} // namespace qore
