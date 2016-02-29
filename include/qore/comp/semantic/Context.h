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
/// \brief Context for semantic analysis.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_CONTEXT_H_
#define INCLUDE_QORE_COMP_SEMANTIC_CONTEXT_H_

#include <string>
#include "qore/comp/ast/Name.h"
#include "qore/comp/DiagManager.h"
#include "qore/comp/SourceManager.h"
#include "qore/comp/Token.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Context for semantic analysis.
 */
class Context {

public:
    /**
     * \brief Constructs an instance.
     * \param srcMgr source manager
     * \param diagMgr diagnostic manager
     */
    Context(SourceManager &srcMgr, DiagManager &diagMgr) : srcMgr(srcMgr), diagMgr(diagMgr) {
    }

    /**
     * \brief Returns the actual name of an identifier.
     * \param token an identifier token
     * \return the name of the identifier
     */
    std::string getIdentifier(const Token &token) const {
        assert(token.type == TokenType::Identifier);
        return srcMgr.get(token.location.sourceId).getRange(token.location.offset, token.length);
    }

    /**
     * \brief Converts a name to a string.
     * \param name the name to convert
     * \return the converted name
     */
    std::string nameToString(const ast::Name &name) const {
        assert(name.isValid());
        std::ostringstream str;

        auto it = name.begin();
        if (!name.isRoot()) {
            str << getIdentifier(*it++);
        }
        while (it != name.end()) {
            str << "::" << getIdentifier(*it++);
        }
        return str.str();
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
    SourceManager &srcMgr;
    DiagManager &diagMgr;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_CONTEXT_H_
