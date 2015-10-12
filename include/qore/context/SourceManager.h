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

#include <memory>
#include <string>
#include <vector>
#include "qore/context/Source.h"
#include "qore/context/SourceId.h"

namespace qore {

/**
 * \brief Manages scripts - their names, buffers and identifiers.
 */
class SourceManager {

public:
    /**
     * \brief Creates a Source from a string.
     * \param name the name of the script
     * \param string the source of the script
     * \return a Source representing given script, remains valid until the SourceManager is destroyed
     */
    //TODO add an optional starting line param
    //TODO join multiple sources together (?)
    Source &createFromString(std::string name, std::string string) {
        return create(std::move(name), std::vector<char>(string.begin(), string.end()));
    }

    /**
     * \brief Creates a Source for standard input.
     *
     * The input is read line by line whenever the scanner consumes a newline character.
     * \return a Source representing standard input, remains valid until the SourceManager is destroyed
     */
    Source &createFromStdin() {
        return create("<stdin>", std::vector<char>(), true);
    }

    /**
     * \brief Creates a Source by reading the contents of a file.
     * \param fileName the name of the file
     * \return a Source representing the script read from the file, remains valid until the SourceManager is destroyed
     */
    Source &createFromFile(std::string fileName);

    /**
     * \brief Returns the name of the source with the given id.
     * \param id the id of the source
     * \return the name of the source identified by `id`
     */
    const std::string &getName(SourceId id) const {
        return sources.at(id.id)->getName();
    }

private:
    Source &create(std::string name, std::vector<char> &&data, bool stdin = false) {
        //XXX needs synchronization
        sources.emplace_back(new Source(std::move(name), SourceId(sources.size()), std::move(data), stdin));
        return *sources.back();
    }

    std::vector<std::unique_ptr<Source>> sources;

    friend class SourceTestHelper;
};

} // namespace qore

#endif /* INCLUDE_QORE_CONTEXT_SOURCEMANAGER_H_ */
