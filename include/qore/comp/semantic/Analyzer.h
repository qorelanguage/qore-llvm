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
/// \brief Semantic analyzer.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_COMP_SEMANTIC_ANALYZER_H_
#define INCLUDE_QORE_COMP_SEMANTIC_ANALYZER_H_

#include "qore/comp/ast/Script.h"
#include "qore/comp/semantic/Namespace.h"

namespace qore {
namespace comp {
namespace semantic {

/**
 * \brief Semantic analyzer.
 */
class Analyzer {

public:
    /**
     * \brief Constructs an instance.
     * \param srcMgr source manager
     * \param diagMgr diagnostic manager
     */
    Analyzer(SourceManager &srcMgr, DiagManager &diagMgr) : context(srcMgr, diagMgr) {
    }

    /**
     * \brief Analyzes the script.
     * \param script the script to analyze
     * \return the root namespace
     */
    std::unique_ptr<Namespace> analyze(ast::Script::Ptr &script);

private:
    Context context;
};

} // namespace semantic
} // namespace comp
} // namespace qore

#endif // INCLUDE_QORE_COMP_SEMANTIC_ANALYZER_H_
