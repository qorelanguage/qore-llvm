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
/// \brief Defines the context of the compiler.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_CONTEXT_H_
#define INCLUDE_QORE_COMP_CONTEXT_H_

#include <string>
#include "qore/comp/DiagManager.h"
#include "qore/comp/SourceManager.h"
#include "qore/comp/String.h"

namespace qore {
namespace comp {

/**
 * \brief Provides access to the core components of the compiler.
 */
class Context {

public:
    /**
     * \brief Creates a new context.
     */
    explicit Context(StringTable &stringTable, DiagManager &diagMgr, SourceManager &srcMgr)
            : stringTable(stringTable), diagMgr(diagMgr), srcMgr(srcMgr) {
    }

    /**
     * \brief Returns the string table.
     * \return the string table
     */
    StringTable &getStringTable() {
        return stringTable;
    }

    /**
     * \brief Returns the diagnostic manager.
     * \return the diagnostic manager
     */
    DiagManager &getDiagMgr() {
        return diagMgr;
    }

    /**
     * \brief Returns the source manager.
     * \return the source manager
     */
    SourceManager &getSrcMgr() {
        return srcMgr;
    }

    /**
     * \brief Reports a diagnostic message.
     * \param diagId the identifier of the diagnostic message
     * \param location the location in the source
     * \return a builder for providing additional info
     */
    DiagBuilder report(DiagId diagId, SourceLocation location) {
        return diagMgr.report(diagId, location);
    }

private:
    Context(const Context &) = delete;
    Context(Context &&) = delete;
    Context &operator=(const Context &) = delete;
    Context &operator=(Context &&) = delete;

private:
    StringTable &stringTable;
    DiagManager &diagMgr;
    SourceManager &srcMgr;
};

} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_CONTEXT_H_
