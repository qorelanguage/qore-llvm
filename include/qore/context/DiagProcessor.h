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
/// \brief Processing of diagnostic messages.
///
//------------------------------------------------------------------------------
#ifndef INCLUDE_QORE_CONTEXT_DIAGPROCESSOR_H_
#define INCLUDE_QORE_CONTEXT_DIAGPROCESSOR_H_

#include <iostream>
#include "qore/context/DiagRecord.h"
#include "qore/context/SourceManager.h"

namespace qore {

/**
 * \brief Interface for diagnostic processors.
 */
class DiagProcessor {

public:
    virtual ~DiagProcessor() = default;

    /**
     * \brief Called for each diagnostic message.
     *
     * The processor can modify the diagnostic message or report it to the user.
     * \param record the diagnostic message to process
     */
    virtual void process(DiagRecord &record) = 0;

protected:
    DiagProcessor() = default;

private:
    DiagProcessor(const DiagProcessor &) = delete;
    DiagProcessor(DiagProcessor &&) = delete;
    DiagProcessor &operator=(const DiagProcessor &) = delete;
    DiagProcessor &operator=(DiagProcessor &&) = delete;
};

/**
 * \brief Basic DiagProcessor that writes messages to the standard error stream.
 */
class DiagPrinter : public qore::DiagProcessor {

public:
    /**
     * \brief Constructs the instance.
     * \param sourceMgr used for getting source names
     */
    DiagPrinter(const SourceManager &sourceMgr) : sourceMgr(sourceMgr) {
    }

    void process(qore::DiagRecord &record) override {
        std::cerr << sourceMgr.getName(record.location.sourceId) << ':' << record.location
                << ": " << record.level << ": " << record.code << ": " << record.message << '\n';
    }

private:
    const SourceManager &sourceMgr;
};

} // namespace qore

#endif // INCLUDE_QORE_CONTEXT_DIAGPROCESSOR_H_
