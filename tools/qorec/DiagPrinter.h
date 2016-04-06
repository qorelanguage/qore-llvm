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
/// \brief TODO file description
///
//------------------------------------------------------------------------------
#ifndef TOOLS_QOREC_DIAGPRINTER_H_
#define TOOLS_QOREC_DIAGPRINTER_H_

#include <utility>

namespace qore {

/**
 * \brief Basic DiagProcessor that writes messages to the standard error stream.
 */
class DiagPrinter : public qore::comp::IDiagProcessor {

public:
    /**
     * \brief Constructs the instance.
     * \param srcMgr used for decoding source locations
     */
    explicit DiagPrinter(const qore::comp::SourceManager &srcMgr) : srcMgr(srcMgr) {
    }

    void process(qore::comp::DiagRecord &record) override {
        qore::comp::Source &src = srcMgr.get(record.location.sourceId);
        std::pair<int, int> l = src.decodeLocation(record.location.offset);
        std::cerr << src.getName() << ':' << l.first << ':' << l.second
                << ": " << record.level << ": " << record.code << ": " << record.message << '\n';
    }

private:
    const qore::comp::SourceManager &srcMgr;
};

} // namespace qore

#endif // TOOLS_QOREC_DIAGPRINTER_H_
