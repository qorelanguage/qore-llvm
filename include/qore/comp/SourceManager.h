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
    SourceManager(DiagManager &diagMgr, std::string includePath = "")
        : diagMgr(diagMgr), includePath(std::move(includePath)) {
    }

    /**
     * \brief Creates a Source from a string.
     * \param name the name of the script
     * \param string the source of the script with no '\0' characters
     * \return a Source representing given script, remains valid until the SourceManager is destroyed
     */
    Source &createFromString(std::string name, std::string string) {
        return create(std::move(name), std::vector<char>(string.begin(), string.end()));
    }

    /**
     * \brief Creates a Source by reading the contents of a file.
     * \param fileName the name of the file
     * \param location the location to use for diagnostic in case the file cannot be read
     * \return a Source representing the script read from the file, remains valid until the SourceManager is destroyed
     */
    Source &createFromFile(std::string fileName, SourceLocation location = SourceLocation());

    /**
     * \brief Returns the name of the source with the given id.
     * \param id the id of the source
     * \return the name of the source identified by `id`
     */
    const std::string &getName(int id) const {
        return sources.at(id)->getName();
    }

private:
    Source &create(std::string name, std::vector<char> &&data);

    DiagManager &diagMgr;
    std::string includePath;
    std::vector<std::unique_ptr<Source>> sources;
};

} // namespace comp
} // namespace qore

#endif /* INCLUDE_QORE_COMP_SOURCEMANAGER_H_ */
