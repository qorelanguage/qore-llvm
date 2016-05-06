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
/// \brief Manager of script sources.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SOURCEMANAGER_H_
#define INCLUDE_QORE_COMP_SOURCEMANAGER_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "qore/comp/DiagManager.h"
#include "qore/comp/Source.h"

namespace qore {
namespace comp {

/**
 * \brief Manages scripts - their names, buffers and identifiers.
 */
class SourceManager {

public:
    /**
     * \brief Constructor.
     * \param diagMgr for reporting diagnostic messages
     * \param includePath the include path to prepend to each file name passed to createFromFile()
     */
    explicit SourceManager(DiagManager &diagMgr, std::string includePath = "")
        : diagMgr(diagMgr), includePath(std::move(includePath)) {
    }

    /**
     * \brief Creates a Source from a string.
     * \param info the source info
     * \param string the source of the script with no '\0' characters
     * \return a Source representing given script, remains valid until the SourceManager is destroyed
     */
    Source &createFromString(const SourceInfo &info, std::string string) {
        return create(info, std::vector<char>(string.begin(), string.end()));
    }

    /**
     * \brief Creates a Source by reading the contents of a file.
     * \param info the source info
     * \param fileName the name of the file
     * \param location the location to use for diagnostic in case the file cannot be read
     * \return a Source representing the script read from the file, remains valid until the SourceManager is destroyed
     */
    Source &createFromFile(SourceInfo &info, std::string fileName, SourceLocation location = SourceLocation());

    /**
     * \brief Returns the substring of given length starting at given location.
     * \param sourceInfo the source info
     * \param offset the offset of the first character of the substring in the source
     * \param length the length of the substring
     * \return the substring of `length` starting at `location`
     */
    std::string getRange(const SourceInfo &sourceInfo, int offset, int length) const {
        auto it = sources.find(&sourceInfo);
        assert(it != sources.end());
        return it->second->getRange(offset, length);
    }

private:
    Source &create(const SourceInfo &info, std::vector<char> data);

    DiagManager &diagMgr;
    std::string includePath;
    std::map<const SourceInfo *, std::unique_ptr<Source>> sources;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SOURCEMANAGER_H_
