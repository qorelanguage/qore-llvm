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
#ifndef INCLUDE_QORE_CONTEXT_SOURCEMANAGER_H_
#define INCLUDE_QORE_CONTEXT_SOURCEMANAGER_H_

#include <string>
#include <vector>
#include "qore/common/Util.h"
#include "qore/context/SourceBuffer.h"

namespace qore {

/**
 * \brief Manages scripts - their names, buffers and identifiers.
 */
class SourceManager {

public:
    /**
     * \brief Creates a SourceBuffer from a string.
     * \param name the name of the script
     * \param string the source of the script
     * \return a SourceBuffer representing given script
     */
    SourceBuffer createFromString(std::string name, std::string string) {
        return SourceBuffer(createId(std::move(name)), string.begin(), string.end());
    }

    /**
     * \brief Creates a SourceBuffer for standard input.
     *
     * The input is read line by line whenever the scanner consumes a newline character.
     * \return a SourceBuffer representing standard input
     */
    SourceBuffer createFromStdin() {
        return SourceBuffer(createId("<stdin>"));
    }

    /**
     * \brief Creates a SourceBuffer by reading the contents of a file.
     * \param fileName the name of the file
     * \return a SourceBuffer representing the script rad from the file
     */
    SourceBuffer createFromFile(std::string fileName);

    /**
     * \brief Returns the name of the source with the given id.
     * \param id
     * \return the name of the source identified by `id`
     */
    const std::string &getName(SourceId id) const {
        return names.at(id.id);
    }

private:
    SourceId createId(std::string name) {
        names.push_back(std::move(name));
        return SourceId(names.size() - 1);
    }

    std::vector<std::string> names;

    FRIEND_FIXTURE(SourceManagerTest);
};

} // namespace qore

#endif /* INCLUDE_QORE_CONTEXT_SOURCEMANAGER_H_ */
